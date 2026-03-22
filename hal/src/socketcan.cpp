#include "socketcan.h"

#include <cstring>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/can.h>
#include <linux/can/raw.h>

SocketCan::SocketCan(const std::string& interface) {
    // Cria socket CAN
    fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd_ < 0) return;

    // Configura interface (ex: vcan0)
    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ - 1);
    ioctl(fd_, SIOCGIFINDEX, &ifr);

    // Faz bind na interface
    struct sockaddr_can addr{};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(fd_);
        fd_ = -1;
        return;
    }

    // Configura socket como não-bloqueante
    int flags = fcntl(fd_, F_GETFL, 0);
    fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
}

SocketCan::~SocketCan() {
    if (fd_ >= 0) close(fd_);
}

bool SocketCan::send(const CanFrame& frame) {
    if (fd_ < 0) return false;

    struct can_frame cf{};
    cf.can_id  = frame.id;
    cf.can_dlc = frame.dlc;
    std::memcpy(cf.data, frame.data, frame.dlc);

    return ::write(fd_, &cf, sizeof(cf)) == sizeof(cf);
}

bool SocketCan::receive(CanFrame& frame) {
    if (fd_ < 0) return false;

    struct can_frame cf{};
    ssize_t nbytes = ::read(fd_, &cf, sizeof(cf));
    if (nbytes <= 0) return false;

    frame.id  = cf.can_id;
    frame.dlc = cf.can_dlc;
    std::memcpy(frame.data, cf.data, cf.can_dlc);
    return true;
}

bool SocketCan::isAvailable() {
    CanFrame frame;
    // Tenta ler sem consumir — peek via MSG_PEEK
    if (fd_ < 0) return false;
    struct can_frame cf{};
    ssize_t nbytes = ::recv(fd_, &cf, sizeof(cf), MSG_PEEK | MSG_DONTWAIT);
    return nbytes > 0;
}
