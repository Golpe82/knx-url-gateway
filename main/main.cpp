#include "../libknx-control/libknx/control/KnxControl.hpp"
#include "../libhttp/libhttp/http.hpp"
#include "../libknx-manage/libknx/manage/KnxManage.hpp"

int main(void)
{
    bool debug = true;
    httplib::Server serveKnx;
    serveKnx.Get(R"(^/([0-9]+)/([0-9]+)/([0-9]+)(-an|-aus|-|\+)?$)", [&](const httplib::Request& req,
                                                            httplib::Response& res) {
        http request;
        knxManage manage;
        knxControl control;
        if(request.SetDataStr(req.matches)) request.print(2);
        else{
            request.print(1);
            switch (manage.ReadLookUp(request.GetGroupaddressStr())){
            case 0:
                manage.print(2);
                if(debug) manage.print(3);
                break;
            case 1:
                if(debug) manage.print(3);
                control.SetData(request.GetRequestStr(), manage.GetDatapointtypeStr()); 
                switch (control.GetDpt()){
                case 1:   
                    control.SendFrame();
                    break;
                case 3:   
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
        }
        res.set_content(manage.xmlResp(request.GetValueStr()), "text/plain");
    });
    serveKnx.listen("0.0.0.0", 1234);
}
