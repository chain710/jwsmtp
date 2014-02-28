// Note that the only valid version of the GPL as far as jwSMTP
// is concerned is v2 of the license (ie v2, not v2.2 or v3.x or whatever),
// unless explicitly otherwise stated.
//
// This file is part of the jwSMTP library.
//
//  jwSMTP library is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; version 2 of the License.
//
//  jwSMTP library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with jwSMTP library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// jwSMTP library
//   http://johnwiggins.net
//   smtplib@johnwiggins.net
//

#include <string>
#include <vector>
#include "compat.h"

namespace jwsmtp
{

#ifdef WIN32
    ScopedSocket::SOCK_TYPE ScopedSocket::BAD_SOCKET = INVALID_SOCKET;
#else
    ScopedSocket::SOCK_TYPE ScopedSocket::BAD_SOCKET = -1;
#endif




// just wrap the call to shutdown the connection on a socket
// this way I don't have to call it this way everywhere.

// This does nothing on unix.
// for windoze only, to initialise networking, snore
void initNetworking()
{
#ifdef WIN32
    class socks
    {
    public:
        bool init()
        {

            WORD wVersionRequested;
            WSADATA wsaData;

            wVersionRequested = MAKEWORD( 2, 0 );
            int ret = WSAStartup( wVersionRequested, &wsaData);
            if(ret) {
                return false;
            }
            initialised = true;
            return true;
        }
        bool IsInitialised() const
        {
            return initialised;
        }
        socks():initialised(false)
        {
            init();
        }
        ~socks()
        {
            if(initialised) {
                shutdown();
            }
        }
    private:
        void shutdown()
        {
            WSACleanup();
        }
        bool initialised;
    };
    static socks s;
#endif
}

bool ScopedSocket::Connect( const SOCKADDR_IN& addr )
{
#ifdef WIN32
    return bool(connect(_sock, (sockaddr*)&addr, addr.get_size()) != SOCKET_ERROR);
#else
    return bool(connect(_sock, (sockaddr*)&addr, addr.get_size()) == 0);
#endif
}
bool ScopedSocket::Create( int domain, int type, int protocol )
{
    _sock = socket(AF_INET, type, protocol);
#ifdef WIN32
    return bool(_sock != INVALID_SOCKET);
#else
    return bool(_sock != -1);
#endif
}
bool ScopedSocket::Send( int &CharsSent, const char *msg, size_t len, int flags )
{
    CharsSent = send(_sock, msg, len, flags);
#ifdef WIN32
    return bool((CharsSent != SOCKET_ERROR));
#else
    return bool((CharsSent != -1));
#endif
}
bool ScopedSocket::Recv( int &CharsRecv, char *buf, size_t len, int flags )
{
    CharsRecv = recv(_sock, buf, len, flags);
#ifdef WIN32
    return bool((CharsRecv != SOCKET_ERROR));
#else
    return bool((CharsRecv != -1));
#endif
}
void ScopedSocket::Closesocket()
{
    if (_sock != BAD_SOCKET) {
#ifdef WIN32
        closesocket(_sock);
#else
        close(_sock);
#endif
    }
    _sock = BAD_SOCKET;
}
} // end namespace jwsmtp

