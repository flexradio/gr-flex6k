
#include <iostream>
#include "../FlexRadio6000.h"
#include <thread>
#include <string>

// template class boost::circular_buffer<float>;

void producer_thread()
{
    std::cout << "hello world from thread Produce" << std::endl;
}

void consumer_thread()
{
    std::cout << "hello world from thread Consume" << std::endl;
}

int main(int argc, char const *argv[])
{

    // spsc_buffer::spsc_semaphore_buffer testBuf(10,200);
    // spsc_semaphore_buffer::reader reader = testBuf.make_reader();
    // auto test=testBuf.get_reader();

    // int out = test->addOne(10);
    // flex::waveformlib_intf* test= new flex::waveformlib_intf();
    const struct timeval timeoutsdf = { .tv_sec = 10, .tv_usec = 0};
    // auto addr = waveform_discover_radio(&timeoutsdf);
    // auto ip_addr = addr->sin_addr.s_addr;
    //         printf("Radio address %d.%d.%d.%d:%d\n",
    //            0xff & ip_addr,
    //            0xff & (ip_addr >> 8),
    //            0xff & (ip_addr >> 16),
    //            ip_addr >> 24,
            //    addr->sin_port);
    
    // waveform_radio_create()
    
    std::cout << "argc = " << argc << std::endl;
    for (int i = 0; i < argc; i++)
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;

    std::cout << "Radio connection testing..." << std::endl;
    struct timeval timeout = {.tv_sec = 10, .tv_usec = 0};
    // waveform_discover_radio(&timeout);

    std::string ip;
    if (argc==1){
        ip = "129.162.166.95";
    }else{
        ip = std::string(argv[1]);
    }



    std::cout << ip << std::endl;
    // FlexRadio6000* radio0 = FlexRadio6000::Connect("129.162.166.95","lpi","95_0","RAW");
    // FlexRadio6000* radio1 = FlexRadio6000::Connect("129.162.166.95","lpi","95_1","RAW");

    // FlexRadio6000* radio2 = FlexRadio6000::Connect("129.162.166.166","LPI1","66_0","RAW");
    std::shared_ptr<FlexRadio6000> radio3 = FlexRadio6000::Connect("129.162.166.166","LPI2","66_1","RAW");
    while(true){
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
     }

    // std::thread th_prod(producer_thread);
    // std::thread th_cons(consumer_thread);

    // radio2

    // FlexRadio6000::Disconnect("129.162.166.166");

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    // FlexRadio6000* radio1 = FlexRadio6000::Connect("129.162.166.166");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // th_prod.join();
    // th_cons.join();
    return 0;
}
