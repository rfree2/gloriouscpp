#pragma once
#ifndef __INCLUDED_H_liboneinstance_hpp 
#define __INCLUDED_H_liboneinstance_hpp 1

#include <thread>
#include <atomic>

#include <cstdio>
#include <cstdlib>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <unistd.h>
#include <pwd.h>

#include "msg_mutex.hpp"
#include "libprocctrl.hpp"

#define _info(X) do { std::cerr<<getpid()<<"/"<<(std::this_thread::get_id())<<" "<<X<<std::endl; } while(0)

namespace nOneInstance {

// major version of this library: (in header/API)
#define nOneInstance_library_version_major "1"

template <class TTag> std::string GetLibraryVersionMajor() { return nOneInstance_library_version_major; }

std::string GetLibraryVersionFull();

const std::string library_version=nOneInstance_library_version_major;

// what type of instance do want, e.g. one per system, or per user, or per directory:
typedef enum { e_range_system=100, e_range_user, e_range_maindir } t_instance_range;

// what is the situation of given instance e.g. inst1, inst2, inst3 - it can be dead, or was not locked/existing 
// and we won the race to it, or we lost (we lost the race, or it was alive etc)
typedef enum { e_instance_i_won=50, e_instance_i_lost=60, e_instance_seems_dead=70, e_instance_unknown=1 } t_instance_outcome;

// TODO C++11 constructor tag forwarding 
		
class cMyNamedMutex : public msg_mutex { // public boost::interprocess::named_mutex {
	private:
		const std::string m_name; // copy of the mutex-name
		bool m_own; ///< do I own this mutex, e.g. do I have the right to unlock it (when exiting)

	public:
		//	using boost::interprocess::named_mutex::named_mutex; ///< forward ALL the constructors o/ http://stackoverflow.com/questions/3119929/forwarding-all-constructors-in-c0x

		cMyNamedMutex(boost::interprocess::create_only_t create_only, const char *name,
				const boost::interprocess::permissions &perm = boost::interprocess::permissions(), size_t msglen=msglen_default);
		cMyNamedMutex(boost::interprocess::open_or_create_t open_or_create, const char *name,
				const boost::interprocess::permissions &perm = boost::interprocess::permissions(), size_t msglen=msglen_default);
		cMyNamedMutex(boost::interprocess::open_only_t open_only, const char *name, size_t msglen=msglen_default);

		~cMyNamedMutex();

		std::string GetName() const; ///< return my mutex-name (e.g. to create another mutex to same mutex-name object)
		void SetOwnership(bool own=true); ///< set ownership (does not clear/unlock if we disown object!)

		static std::string EscapeMutexName(const std::string in); ///< escape any string so it becames a valid name (and part of name) of named-mutex object
		static std::string EscapeMutexNameWithLen(const std::string in); ///< escape also, but prepends the length of string to make this immune to any corner-case bugs
		void Print(std::ostream &out) const;
};

std::ostream& operator<<(std::ostream &out, const cMyNamedMutex & obj);


class cInstancePingable {
	private:
		std::unique_ptr< std::thread> m_thread; ///< the thread with pong loop that replies to pings
		std::unique_ptr< cMyNamedMutex > m_pong_obj; ///< the pong object. UNLOCK it ONLY to signall that we are alive (not on exit!)
		std::atomic<bool> m_run_flag; ///< should we run? or stop
		std::atomic<bool> m_go_flag; ///< set to true to really start thread. do NOT use any data of this class other then destructor after
		std::atomic<bool> m_hang_pings; ///< halt responding to pings for now. Debug, see HangPings()

		const std::string m_mutex_name; ///< mutex-name, will be used for the name of PING object
		boost::interprocess::permissions m_mutex_perms; ///< what should be the permissions of the PING object

	public:
		cInstancePingable(const std::string &base_mutex_name, const boost::interprocess::permissions & mutex_perms);
		~cInstancePingable(); ///< destructor will close down the thread nicelly
		void PongLoop();
		void Run();
		void HangPings(bool hang=true); //< halt responding to pings, simulate we are hanged (e.g. for debug)

		static std::string BaseNameToPingName(const std::string &base_mutex_name);
};



class cInstanceObject {
	protected:
		const t_instance_range m_range; ///< range of this instance
		const std::string m_program_name; ///< a name for the program as assigned by developer so other instances will find you; NOT executable name/pid/std related.

		std::unique_ptr< cMyNamedMutex > m_curr_mutex; ///< access the current mutex (e.g. m3, m4... if m1 was taken)
		std::unique_ptr< cMyNamedMutex > m_m1_mutex; ///< access the m1 mutex (once we are sure we are the leader)

		std::unique_ptr< cInstancePingable > m_pingable_curr; ///< we will respond to ping (as current Mc, e.g. m1)
		std::unique_ptr< cInstancePingable > m_pingable_m1; ///< we will respond to ping (as the leader m1)

		t_instance_outcome TryToBecomeInstance(int inst); ///< test instance, tries to become it

		bool PingInstance(const std::string &base_name,  const boost::interprocess::permissions & permissions); ///< tries to ping instance called base_name. Will create ping object with given permissions

		std::string GetUserName() const;
		std::string GetDirName() const;

		std::string GetProcessIdentification() const;

	public:
		cInstanceObject(t_instance_range range, const std::string &program_name);
		virtual ~cInstanceObject();

		cInstanceObject(const cInstanceObject &)=delete; // do not copy!
		cInstanceObject& operator=(const cInstanceObject &)=delete; // do not copy!
		cInstanceObject& operator=(cInstanceObject &&)=delete; // do not copy!

		bool BeTheOnlyInstance(); ///< try to be the leader - the only instance. Return false if this failed and other instance is running.
		void HangPings(bool hang=true); //< halt responding to pings, simulate we are hanged (e.g. for debug)

};


} // namespace nOneInstance


#endif

