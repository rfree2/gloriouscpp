#ifndef CSAFEMUTEX_H_
#define CSAFEMUTEX_H_

#include <string>
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <type_traits>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/permissions.hpp>

class warning_already_unlocked : public std::exception { };

/** 
@description a mutex that is safe (e.g. against double unlock, double lock), and can pass a messae
@note this is safe way, because: http://lists.boost.org/boost-users/2012/03/73888.php
*/
class msg_mutex
{
public:

	typedef char t_msg_char; // one character of the message - it must be pod
	typedef std::vector< t_msg_char > t_msg; // the message
	static_assert(std::is_pod<t_msg_char>::value, "The character type must be a POD type.");

	static constexpr int msglen_default = 8192;
	static char msgtxt_default; /// empty message to be used in lock
	static_assert(std::is_pod<decltype(msgtxt_default)>::value, "The default value must a POD (e.g. a single character).");

	msg_mutex(const char* name, size_t msglen=msglen_default);

	msg_mutex(boost::interprocess::create_only_t create_only, const char *name,
			const boost::interprocess::permissions &perm = boost::interprocess::permissions(), size_t msglen=msglen_default);
	msg_mutex(boost::interprocess::open_or_create_t open_or_create, const char *name,
			const boost::interprocess::permissions &perm = boost::interprocess::permissions(), size_t msglen=msglen_default);
	msg_mutex(boost::interprocess::open_only_t open_only, const char *name, size_t msglen=msglen_default);

	~msg_mutex();

	bool remove(); // TODO also a static member function for this
	std::string get_name();

	// mutex-like api:
	void lock(); // lock mutex, block thread if mutex is locked
	void unlock(); // safe unlock, if mutex is unlocked does nothing
	bool try_lock(); // tries to lock the mutex, returns false when the mutex is already locked, returns true when success
	bool try_unlock();
	bool timed_lock(const boost::posix_time::seconds &sec); // return true when locks

	bool is_locked();
	
	// message-like api:
	void lock_msg(const t_msg& msg); // lock mutex, block thread if mutex is locked
	t_msg unlock_msg(); // safe unlock, if mutex is unlocked does nothing
	bool try_lock_msg(const t_msg& msg); ///< tries to lock the mutex, returns false when the mutex is already locked, returns true when success
	bool try_unlock_msg(t_msg & msg_out ); ///< returns if unlock succeeded and if yes then sets msgout to the read message
	bool timed_lock_msg(const boost::posix_time::seconds &sec, const t_msg& msg); // return true when locks

private:
	const std::string mName;
	size_t m_msglen; ///< what message length was configured here
	boost::interprocess::message_queue mMsgQueue;
	t_msg mBuffer; ///< buffer e.g. for incomming message
};

#endif /* CSAFEMUTEX_H_ */

