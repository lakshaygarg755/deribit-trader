#include "wssheader.hpp"
#include "func.hpp"
#include <thread>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

extern std::string CLIENT_ID;
extern std::string CLIENT_SECRET;

int main() {
    try {
        WebSocketClient ws;

        json auth = {
            {"jsonrpc", "2.0"},
            {"id", 9929},
            {"method", "public/auth"},
            {"params", {
                {"grant_type", "client_credentials"},
                {"client_id", CLIENT_ID},
                {"client_secret", CLIENT_SECRET}
            }}
        };

        auto response = ws.send(auth);
        std::string token = response["result"]["access_token"];
        cout << token << endl;
        
        int choice;
        while (true) {
            displayMenu();
            cin >> choice;
    
            switch (choice) {
                case 1: {  
                    auto order = place_order();
                    response = ws.send(order);
                    cout << "Order state: " << response["result"]["order"]["order_state"] << endl;
                    cout << "Order ID: " << response["result"]["order"]["order_id"] << endl;
                    break;
                }
                case 2: {  
                    auto cancelOrder = cancel();
                    response = ws.send(cancelOrder);
                    cout << "Cancel Response: " << response["result"]["order_state"] << endl;

                    break;
                }
                case 3: {  
                    auto modified = modify();
                    response = ws.send(modified);
                    cout << "Order replaced: " << response["result"]["order"]["replaced"] << endl;
                    break;
                }
                case 4: {  
                    auto orderBook = ws.send(get_order_book());
                    printOrderBook(orderBook);
                    break;
                }
                case 5: {  
                    auto positions = ws.send(get_positions_request());
                    cout << "Positions: " << positions["result"].dump(4) << endl;
                    break;
                }
                case 6: { 
                    ws.subscribe(subscribe());
                    break;
                }
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        }
        ws.close();
        return 0;
    }
    catch (const boost::system::system_error& e) {
        std::cerr << "Boost System: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
