#include "msg_mutex.hpp"
#include "libsimpleconvert.hpp"

#include <thread>

#ifndef _internal_dbg
	#define _internl_dbg(X) do { std::cerr<<" "<<__FILE__<<getpid()<<"/"<<(std::this_thread::get_id())<<" "<<X<<std::endl; } while(0)
#endif

msg_mutex::msg_mutex(const char * name, size_t msglen)
	:
	mName(name),
	m_msglen(msglen),
	mMsgQueue(boost::interprocess::open_or_create, name, 1, m_msglen),
	mBuffer(m_msglen)
{
	_internal_dbg("Constructed mutex this="<<this<<" m_msglen="<<m_msglen<<" name="<<mName);
}

msg_mutex::msg_mutex(boost::interprocess::create_only_t create_only,
		const char *name,
		const boost::interprocess::permissions &perm, size_t msglen)
:
	mName(name),
	m_msglen(msglen),
	mMsgQueue(create_only, name, 1, m_msglen, perm),
	mBuffer(m_msglen)
{
	_internal_dbg("Constructed mutex this="<<this<<" m_msglen="<<m_msglen<<" name="<<mName);
}

msg_mutex::msg_mutex(boost::interprocess::open_or_create_t open_or_create,
		const char *name,
		const boost::interprocess::permissions &perm, size_t msglen)
:
	mName(name),
	m_msglen(msglen),
	mMsgQueue(open_or_create, name, 1, m_msglen, perm),
	mBuffer(m_msglen)
{
	_internal_dbg("Constructed mutex this="<<this<<" m_msglen="<<m_msglen<<" name="<<mName);
}

msg_mutex::msg_mutex(boost::interprocess::open_only_t open_only, const char *name, size_t msglen)
:
	mName(name),
	m_msglen(msglen),
	mMsgQueue(open_only, name),
	mBuffer(0)
{
	_internal_dbg("Constructed mutex this="<<this<<" m_msglen="<<m_msglen<<" name="<<mName);
}

msg_mutex::~msg_mutex()
{
	_internal_dbg("Destructing mutex this="<<this<<" m_msglen="<<m_msglen<<" name="<<mName);
}

void msg_mutex::lock() {
	_internal_dbg("LOCK with default msg="<<msgtxt_default<<" m_msglen="<<m_msglen);
	mMsgQueue.send( & msgtxt_default, sizeof(msgtxt_default), 0); // we send any data (the default tiny message) it must be POD
}

void msg_mutex::lock_msg(const t_msg& msg) {
	_internal_dbg("LOCK with msg="<<vec_to_str(msg)<<" m_msglen="<<m_msglen);
	mMsgQueue.send( msg.data() , sizeof(msg.at(0)) * msg.size(), 0);
}

bool msg_mutex::try_lock() {
	_internal_dbg("TRY-LOCK with default msg="<<msgtxt_default<<" m_msglen="<<m_msglen);
	return mMsgQueue.try_send( & msgtxt_default, sizeof(msgtxt_default), 0); // we send any data (the default tiny message) it must be POD
}

bool msg_mutex::try_lock_msg(const t_msg & msg) {
	_internal_dbg("TRY-LOCK with msg="<<vec_to_str(msg)<<" m_msglen="<<m_msglen
		<<" get_max_msg_size()=" << mMsgQueue.get_max_msg_size() << " get_max_msg()=" << mMsgQueue.get_max_msg());
	return mMsgQueue.try_send( msg.data(), msg.size()*sizeof(msg.at(0)), 0);
}

void msg_mutex::unlock() {
	boost::interprocess::message_queue::size_type recvd_size;
	unsigned int recvd_priority;
	if (!mMsgQueue.try_receive( mBuffer.data(), mBuffer.size()*sizeof(mBuffer.at(0)), recvd_size, recvd_priority))	{
		throw warning_already_unlocked();
	}
}

msg_mutex::t_msg msg_mutex::unlock_msg() {
	boost::interprocess::message_queue::size_type recvd_size;
	unsigned int recvd_priority;
	if (!mMsgQueue.try_receive( mBuffer.data(), mBuffer.size()*sizeof(mBuffer.at(0)), recvd_size, recvd_priority)) {
		throw warning_already_unlocked();
	}
	t_msg received( mBuffer.cbegin() , mBuffer.cbegin()+recvd_size); // move the actually used data to a new variable
	return received;
}

bool msg_mutex::try_unlock() {
	try {
		unlock();
		return true;
	}
	catch (...) {	return false;	}
}

bool msg_mutex::try_unlock_msg(t_msg & msg_out ) {
	try {
		msg_out = unlock_msg();
		return true;
	}
	catch (...) {	return false;	}
}

bool msg_mutex::timed_lock(const boost::posix_time::seconds &sec) {
	return mMsgQueue.timed_send(&mBuffer, sizeof(int), 0, boost::posix_time::second_clock::universal_time() + sec);
}

bool msg_mutex::timed_lock_msg(const boost::posix_time::seconds &sec, const t_msg& msg) {
	return mMsgQueue.timed_send(msg.data(), sizeof(msg.at(0))*msg.size(), 0, boost::posix_time::second_clock::universal_time() + sec);
}

bool msg_mutex::is_locked() {
	if (mMsgQueue.get_num_msg() == 1) {	return true; }
	else { return false; }
}


bool msg_mutex::remove() {
	return boost::interprocess::message_queue::remove(mName.c_str());
}

std::string msg_mutex::get_name() {
	return mName;
}


char msg_mutex::msgtxt_default = 'L'; // empty message to be used in lock


#undef _internal_dbg

