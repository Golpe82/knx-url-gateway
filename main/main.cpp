#include "../libknx-control/libknx/control/KnxControl.hpp"
#include "../libhttp/libhttp/http.hpp"
#include "../libknx-manage/libknx/manage/KnxManage.hpp"

// Example url: http://10.110.16.112:4321/1/6/1-send_celsius=3
// (/([0-9]+)/([0-9]+)/([0-9]+)(-an|-aus|-minus|-plus|temp=)?$)
const int production_port = 1234;
const int dev_port = 4321;
// -text=004917642048313

int main(void)
{
    bool debug = true;
    httplib::Server serveKnx;
    // TODO regex should accept for -send_celsius negative and float numbers.
    // e.g.: -send_celsius= -25 or -send_celsius= 21.2 or -send_celsius=-10.23
    serveKnx.Get(R"(^/([0-9]+)/([0-9]+)/([0-9]+)(-an|-aus|-minus|-plus|-send_celsius=\b([0-9]|[1-4][0-9])\b|-text=\b(.{1,50})\b)?$)", [&](const httplib::Request &req,
                                                                                                                                    httplib::Response &res){
            http request;
            knxManage manage;
            knxControl control;

            if(request.SetDataStr(req.matches)) request.print(2);
            else
            {
                request.print(1);
                switch (manage.ReadLookUp(request.GetGroupaddressStr()))
                {
                    case 0:
                        manage.print(2);
                        if(debug) manage.print(3);
                        break;
                    case 1:
                        if(debug) manage.print(3);
                        std::cout << "dpt: " << manage.GetDatapointtypeStr() << std::endl;
                        control.SetData(request.GetRequestStr(), manage.GetDatapointtypeStr());
                        switch (control.GetDpt()){
                        case 1: //DPT1
                            control.SendFrame();
                            res.set_content(manage.xmlResp(request.GetValueStr()), "text/plain");
                            break;
                        case 3: //DPT3
                            control.SendFrame();
                            break;
                        case 9: //DPT9
                            control.SendFrame();
                            break;
                        case 16: //DPT16
                            control.SendFrame();
                            break;
                        default:
                            std::cout<<"not found"<<std::endl;
                            break;
                        }
                        break;
                    case 2:
                        manage.print(1);
                        if(debug) manage.print(3);
                        break;
                    default:
                        break;
                }
            } });

    serveKnx.listen("0.0.0.0", production_port);
}
