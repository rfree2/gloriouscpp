#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

#include <thread>
#include <atomic>

#include <cstdio>
#include <cstdlib>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <unistd.h>
#include <pwd.h>

#include "liboneinstance.hpp"
#include "libsimpleconvert.hpp"


// minor version (implementation) - number
#define nOneInstance_library_version_minor 2
// minor version (implementation) - number - is it WIP (or else it is a release)
#define nOneInstance_library_version_minor_wip 1


#define _info(X) do { std::cerr<<getpid()<<"/"<<(std::this_thread::get_id())<<" "<<X<<std::endl; } while(0)


namespace nOneInstance {

std::string GetLibraryVersionFull() {
	return "TODO"; // TODO
}



cMyNamedMutex::cMyNamedMutex(boost::interprocess::create_only_t, const char * name, const boost::interprocess::permissions & permissions, size_t msglen)
: msg_mutex(boost::interprocess::create_only_t(), name, permissions, msglen), m_name(name), m_own(false)
{ }

cMyNamedMutex::cMyNamedMutex(boost::interprocess::open_or_create_t, const char * name, const boost::interprocess::permissions & permissions, size_t msglen)
: msg_mutex(boost::interprocess::open_or_create_t(), name, permissions, msglen), m_name(name), m_own(false)
{ }

cMyNamedMutex::cMyNamedMutex(boost::interprocess::open_only_t, const char * name, size_t msglen) 
: msg_mutex(boost::interprocess::open_only_t(), name, msglen), m_name(name), m_own(false)
{ }

cMyNamedMutex::~cMyNamedMutex() {
	_info("Destructor of named mutex in this="<<this<<" m_own="<<m_own<<" m_name="<<m_name);
	if (m_own) {
		_info("UNLOCKING on exit for m_name="<<m_name);
		auto msg = this->unlock_msg();
		std::ostringstream oss; for(auto ch:msg) oss<<ch; std::string str=oss.str();
		_info("UNLOCKED, with finall msg=" << str );
	}
}

std::string cMyNamedMutex::GetName() const {
	return m_name;
}

void cMyNamedMutex::SetOwnership(bool own) {
	m_own = own;
}

std::string cMyNamedMutex::EscapeMutexName(const std::string in) {
	// TODO lambda/algorithm
	std::string out;
	for (unsigned char c:in) {
		if (  ((c>='a')&&(c<='z')) || ((c>='0')&&(c<='9')) ) out += std::string(1,c);
		else {
			std::ostringstream oss;
			oss<<'_'<<std::hex<<((int)c);
			out += oss.str();
		}
	}
	return out;
}

std::string cMyNamedMutex::EscapeMutexNameWithLen(const std::string in) {
	std::ostringstream oss;
	oss << "l" << in.length() << "_" << EscapeMutexName(in);
	return oss.str();
}

void cMyNamedMutex::Print(std::ostream &out) const {
	out << "{named_mutex " << (void*)this << " " << m_name << ( m_own ? " (OWNER)":"" ) << "}" ;
}

std::ostream& operator<<(std::ostream &out, const cMyNamedMutex & obj) {
	obj.Print(out);
	return out;
}


cInstancePingable::cInstancePingable(const std::string &base_mutex_name, const boost::interprocess::permissions & mutex_perms)
: m_run_flag(false), m_go_flag(false), m_hang_pings(false),
m_mutex_name(BaseNameToPingName(base_mutex_name)), m_mutex_perms(mutex_perms)
{ 
	_info("Constructed cInstancePingable for m_mutex_name="<<m_mutex_name);
}


cInstancePingable::~cInstancePingable() {
	m_run_flag=false; // signall everyone, including the thread that it should stop

	if (m_thread) { // if thread runs (not null)
		_info("Joining the thread");
		m_thread->join(); 
		_info("Joining the thread - done");
	}

}

std::string cInstancePingable::BaseNameToPingName(const std::string &base_mutex_name) {
	std::string a = base_mutex_name;
	a += "_PING";
	_info("PING NAME will be: a="<<a);
	return a;
}

void cInstancePingable::PongLoop() {

	const int time_latency_wakeup = 200; // various wakeup latency, like at beginning and when hanged
	const int time_latency_pong = 500; // how often to send pong

	while (!m_go_flag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(time_latency_wakeup));
	}
	
	_info("PongLoop - begin for m_mutex_name="<<m_mutex_name);
	const int recreate_trigger = 1000; // how often to recreate the object
	int need_recreate = recreate_trigger; // should we (re)create the mutex object now - counter. start at high level to create it initially
	while (m_run_flag) {
		_info("pong loop...");

		while (m_hang_pings && m_run_flag) {
			std::this_thread::sleep_for(std::chrono::milliseconds(time_latency_wakeup));
		}

		// create object if needed
		++need_recreate; // so we will recreate it from time to time
		if (need_recreate >= recreate_trigger) {
			_info("pong: CREATING the object now for m_mutex_name="<<m_mutex_name);
			m_pong_obj.reset( 
				new cMyNamedMutex ( boost::interprocess::open_or_create, m_mutex_name.c_str(), m_mutex_perms ) 
			);
			_info("pong: CREATED object has name: " << m_pong_obj->GetName() );
			need_recreate=false;
		}

		try {
			_info("unlocking: "<<m_pong_obj->GetName());
			m_pong_obj->unlock(); ///< this signals that we are alive <--- ***
			_info("***PONG*** - unlock worked now");
		} 
		catch(warning_already_unlocked) {
			_info("... was already unlocked");
		}
		catch(...) { 
			_info("WARNING: unlocking failed - exception: need to re-create the object probably");
			need_recreate = recreate_trigger; 
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(time_latency_pong)); // sleep TODO config
	}

	_info("PongLoop - end");
}

void cInstancePingable::Run() {
	if (m_run_flag) throw std::runtime_error("Can not Run already running object");
	_info("STARTING pong reply loop for m_mutex_name="<<m_mutex_name<<" - starting");
	m_run_flag=true; // allow it to run
	m_thread.reset( new std::thread(  [this]{ this->PongLoop(); }     ) ); // start the thread and start pong-loop inside it
	_info("STARTING pong reply loop for m_mutex_name="<<m_mutex_name<<" - done");
	m_go_flag=true; // fire it up now. we will not touch any data here
}

void cInstancePingable::HangPings(bool hang) {
	_info("Ok, " + std::string( hang ? "" : "UN-" ) + "hanging the pings here in " << ((void*)this) );
	m_hang_pings=hang;
} 





cInstanceObject::cInstanceObject(t_instance_range range, const std::string &program_name)
: m_range(range), m_program_name(program_name)
{ }

cInstanceObject::~cInstanceObject() {
	_info("Destructor of cInstanceObject at this="<<this);
}

bool cInstanceObject::BeTheOnlyInstance() {
	int instance=1;
	t_instance_outcome outcome = e_instance_unknown; 
	while (!(  outcome==e_instance_i_won  ||  outcome==e_instance_i_lost )) { // untill we win or lose
		outcome = TryToBecomeInstance(instance);
		if (outcome==e_instance_seems_dead) {
			_info("*** Previous instance at instance="<<instance<<" was dead.");
			++instance;
		}
	}
	return outcome==e_instance_i_won ;
}

bool cInstanceObject::PingInstance( const std::string &base_name, const boost::interprocess::permissions &permissions) {
	_info("Will ping name="<<base_name);
	const int dead_threshold = 10; // TODO config how sure we must be that other thread is dead, how many times (number) it must fail to reply
	const int ping_wait_ms = 500; // TODO config how long we wait for one reply

	std::unique_ptr< cMyNamedMutex > ping_mutex( 
		new cMyNamedMutex(
			boost::interprocess::open_or_create,
//			"name1",
			cInstancePingable::BaseNameToPingName( base_name ).c_str() , // the PONG name
			permissions
		)
	);

	int dead_confidence; // are we sure no one is replying
	bool firstlocked=0; // did we locked it even once

	dead_confidence=0; 
	_info("Locking Ping first time. ping_mutex=" << *ping_mutex);
	while (!firstlocked) {
		//firstlocked = ping_mutex->timed_lock(boost::posix_time::second_clock::universal_time() + boost::posix_time::milliseconds(ping_wait_ms * dead_threshold));
		firstlocked = ping_mutex->try_lock();
		//(boost::posix_time::second_clock::universal_time() + boost::posix_time::milliseconds(ping_wait_ms * dead_threshold));
		if (!firstlocked) {
			std::this_thread::sleep_for(std::chrono::milliseconds(ping_wait_ms)); // TODO sleep just the reamining time after timed_lock internal wait
			dead_confidence++;
			_info("Can not lock it even the first time - probably other process is dead? dead_confidence="<<dead_confidence<<" of " <<dead_threshold);
			if (dead_confidence >= dead_threshold) {
				_info("Giving up - seems dead, dead_confidence="<<dead_confidence);
				return false; // can not get even one lock - so we do not see even one unlock - so other instance seems dead
			}
		} 
	}
	// lock it once (so it waits for open spot to request ping) - unless it's stucked for very long
	// because if it's very long then the instance hanged (and someone else locked it now or previously) 
	// so to await infinite wait
	_info("***PING*** is sent: " << *ping_mutex);

	bool relocked = false; 

	long int pings_sent=0;
	dead_confidence=0;
	while (dead_confidence < dead_threshold) {
		try {
			++pings_sent;
			_info("********************************************");
			_info("Trying to relock ping_mutex" << *ping_mutex);
			relocked = ping_mutex->try_lock();
			_info("relocked = " << relocked );
		} catch(...) { } 
		if (relocked) {
			_info("***PONG*** received - thread is ALIVE");
			// TODO or the cleanup process recreated empty lock? better test once more
			return true; // the instance is alive!
		}
		_info("No-reply to ping");
		dead_confidence++; // seems dead
		
		std::this_thread::sleep_for(std::chrono::milliseconds(ping_wait_ms));
	}
	_info("Instance seems dead after pings_sent="<<pings_sent);
	return false;
}

t_instance_outcome cInstanceObject::TryToBecomeInstance(int inst) { // test instance, tries to become it
	_info("Trying instance inst="<<inst);

	const std::string igrp_name = std::string("instancegroup_") 
		+ m_program_name // testprogram
		+ ( (m_range==e_range_user) ? ("_user_n_"+cMyNamedMutex::EscapeMutexNameWithLen(GetUserName())) : ( "_user_ANY" ) ) // testprogram_alice  testprogram
		+ ( (m_range==e_range_maindir) ? ("_dir_n_"+cMyNamedMutex::EscapeMutexNameWithLen(GetDirName())) : ( "_dir_ANY" )   )
	;

	_info("igrp_name=" << igrp_name);

	boost::interprocess::permissions mutex_curr_perms;
	if (m_range==e_range_system) mutex_curr_perms.set_unrestricted(); // others users on same system need to synchronize with this

	std::ostringstream mutex_name_str; mutex_name_str << igrp_name << "_instM" << inst;
	std::string mutex_name = mutex_name_str.str();

	// mutex_name="name1foo1";
	_info("Will try to become instance as mutex_name="<<mutex_name);

	m_curr_mutex.reset( new cMyNamedMutex ( boost::interprocess::open_or_create, mutex_name.c_str(), mutex_curr_perms ) );
	// we created this mutex or it existed

	bool curr_locked = m_curr_mutex->try_lock_msg(str_to_vec("La"));// GetProcessIdentification() ) );
	if (curr_locked) { // we locked it! 
		_info("WE BECOME INSTANCE: Created and locked the Mc - we became the instance at inst="<<inst);
		m_curr_mutex->SetOwnership(true); // I own this mutex e.g. M5

		// I am probably the leader
		
		// ... TODO ... ping all lower numbers are they responding after all to ping ...


		_info("I will start responding to pings (curr)");
		m_pingable_curr.reset(new cInstancePingable( mutex_name, mutex_curr_perms )); // start responding to ping
		m_pingable_curr->Run();

		return e_instance_i_won; // we created and locked (or just locked - reclaimed) an instance
	}

	_info("Can not create/lock the Mc so we lost or it is dead. inst="<<inst);
	bool other_is_alive = PingInstance(mutex_name, mutex_curr_perms);

	if (! other_is_alive) {
		_info("The other instance is dead!");
		return e_instance_seems_dead;
	}
	
	return e_instance_i_lost;
}

std::string cInstanceObject::GetUserName() const {
  uid_t uid = geteuid ();
	struct passwd *pw = getpwuid (uid);
  if (pw) {
  	return pw->pw_name; // TODO escape characters
	}
	return "UNKNOWN";
}

std::string cInstanceObject::GetDirName() const {
	const size_t maxlen = 8192;
	char buf[maxlen];
	char * s1 = getcwd(buf,maxlen);
	if (s1) return s1;
	return "UNKNOWN";
}

void cInstanceObject::HangPings(bool hang) {
	_info("Ok, " + std::string( hang ? "" : "UN-" ) + "hanging the pings");
	if (m_pingable_curr) m_pingable_curr->HangPings(hang);
	if (m_pingable_m1) m_pingable_m1->HangPings(hang);
}


std::string cInstanceObject::GetProcessIdentification() const {
	std::ostringstream oss;
	oss << getpid() ;
	oss << " ";
	oss << "programname"; // TODO @robert
	return oss.str();
}

} // namespace


