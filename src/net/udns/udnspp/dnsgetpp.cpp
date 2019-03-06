
#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstdlib>
#include <cstdarg>

#ifdef WINDOWS
#include <windows.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <arpa/inet.h>
#endif

#include <udnspp/dnsresolver.h>

#define PROGNAME "dnsgetpp"
static bool exit_thread = false;

static void die(int errnum, const char *fmt, ...) 
{
  va_list ap;
  fprintf(stderr, "%s: ", PROGNAME);
  va_start(ap, fmt); vfprintf(stderr, fmt, ap); va_end(ap);
  if (errnum) fprintf(stderr, ": %s\n", strerror(errnum));
  else putc('\n', stderr);
  fflush(stderr);
  exit(1);
}

void handle_dns_av4(const udnspp::DNSARecordV4& rr, void* pUserData)
{
  if (exit_thread)
    return;

  std::string qname = *(static_cast<std::string*>(pUserData));
  if (!rr.getRecords().empty())
  {
    std::cout << "======A Record=====" << std::endl;
    std::cout << "QName: " << rr.getQName() << std::endl;
    std::cout << "CName: " << rr.getCName() << std::endl;
    std::cout << "Address: " << rr.getRecords().front() << std::endl;
    std::cout << "TTL: " << rr.getTTL() << std::endl;
  }
  else
  {
    std::cout << qname << " did not yield any A Record result." << std::endl;
  }
}

void handle_dns_srv(const udnspp::DNSSRVRecord& srv_rr, void* pUserData)
{
  if (exit_thread)
    return;

  std::string qname = *(static_cast<std::string*>(pUserData));
  if (!srv_rr.getRecords().empty())
  {
    std::cout << "======SRV Record=====" << std::endl;
    std::cout << "QName: " << srv_rr.getQName() << std::endl;
    std::cout << "CName: " << srv_rr.getCName() << std::endl;
    std::cout << "Address: " << srv_rr.getRecords().front().name << std::endl;
    std::cout << "TTL: " << srv_rr.getTTL() << std::endl;
  }
  else
  {
    std::cout << qname << " did not yield any SRV Record result." << std::endl;
  }
}

void thread_sleep(unsigned long milliseconds)
/// Pause thread execution for certain time expressed in milliseconds
{
  timeval sTimeout = { (long int)(milliseconds / 1000), (long int)((milliseconds % 1000) * 1000) };
  select(0, 0, 0, 0, &sTimeout);
}

void thread_loop1(udnspp::DNSResolver* resolver, std::string qname)
{
  while (!exit_thread)
  {
#ifdef WINDOWS
    ::Sleep(10);
#else
    usleep(10000);
#endif
    resolver->resolveA4(qname, 0, boost::bind(handle_dns_av4, _1, _2), (void *)&qname);
  }

}


void thread_loop2(udnspp::DNSResolver* resolver, std::string qname)
{
  while (!exit_thread)
  {
#ifdef WINDOWS
    ::Sleep(10);
#else
    usleep(10000);
#endif
    udnspp::DNSARecord rr = resolver->resolveA4(qname, 0);
    if (!rr.getRecords().empty())
    {
      std::cout << "======A Record=====" << std::endl;
      std::cout << "QName: " << rr.getQName() << std::endl;
      std::cout << "CName: " << rr.getCName() << std::endl;
      std::cout << "Address: " << rr.getRecords().front() << std::endl;
      std::cout << "TTL: " << rr.getTTL() << std::endl;
    }
    else
    {
      std::cout << qname << " did not yield any A Record result." << std::endl;
    }
  }
}

int main(int argc, char** argv)
{
  std::string qname;
  if (argc >= 2)
  {
    qname = argv[1];
  }
  else
  {
    die(0, "Please provide a domain argument.  Example: ./dnsget ossapp.com");
  }

  udnspp::DNSResolver resolver;
  resolver.start();

  std::ostringstream srvudp;
  srvudp << "_sip._udp." << qname;

  resolver.resolveA4(qname, 0, boost::bind(handle_dns_av4, _1, _2), (void *)&qname);
  resolver.resolveSRV(srvudp.str(), 0, boost::bind(handle_dns_srv, _1, _2), (void *)&qname);
  
  




  boost::thread t1(boost::bind(thread_loop1, &resolver, qname));
  boost::thread t2(boost::bind(thread_loop2, &resolver, qname));
  boost::thread t3(boost::bind(thread_loop2, &resolver, qname));
  
  char c;
  std::cin.get(c);

  exit_thread = true;
  t1.join();
  t2.join();
  t3.join();
  
  resolver.stop();

  exit(0);
}