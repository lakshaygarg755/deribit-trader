#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

json place_order() {
    string order_type;
    string instrument;
    double price, amount;

    // Prompt the user for input
    cout << "Enter the order type (limit/market): ";
    cin >> order_type;

    if (order_type != "limit" && order_type != "market") {
        cerr << "Invalid order type!" << endl;
        return json();
    }

    cout << "Enter the instrument (e.g., BTC-PERPETUAL): ";
    cin >> instrument;

    cout << "Enter the amount: ";
    cin >> amount;
    

    // For limit orders, ask for the amount
    if (order_type == "limit") {
        cout << "Enter the price: ";
        cin >> price;
    }

    // Construct the params JSON object
    json params = {
        {"type", order_type},
        {"instrument_name", instrument},
        {"amount", amount}
    };

    // Include amount only for limit orders
    if (order_type == "limit") {
        params["price"] = price;
    }

    // Construct the full JSON-RPC request
    json order = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "private/buy"},
        {"params", params}
    };

    std::cout << order.dump() << endl;

    return order;
}

json cancel(){
    json cancel= {
        {"jsonrpc" , "2.0"},
        {"id" , 4214},
        {"method" , "private/cancel"},
      };

    string orderid;
    cout << "Enter the order id: ";
    cin >> orderid;
    cancel["params"]["order_id"] = orderid;
    cout << cancel.dump() << endl;
    return cancel;
}

json modify(){

    string orderid,price,amount;
    cout << "Enter the order id: ";
    cin >> orderid;
    cout << "Enter the new price: ";
    cin>> price;
    cout << "Enter the new amount: ";
    cin >> amount;
    json params = {
        {"order_id" , orderid},
        {"amount" , amount},
        {"price" , price},
        {"post_only" , false}
    };
    json modified = {
        {"jsonrpc" , "2.0"},
        {"id" , 5678},
        {"method" , "private/edit"},
        {"params" , params}
    };
    return modified;
}

json get_order_book(){
    string instrument_name;
    cout << "Enter the instrument name: ";
    cin >> instrument_name;

    json order = {
        {"jsonrpc" , "2.0"},
        {"id" , 8772},
        {"method" , "public/get_order_book"},
        {"params" , {
                {"instrument_name" , instrument_name},
                {"depth" , 5}
            }
        }
    };

    return order;

}


void printOrderBook(const json& orderBook) {
    if (!orderBook.contains("result")) {
        cout << "Invalid order book format!" << endl;
        return;
    }

    json result = orderBook["result"];

    cout << "Asks\n";
    cout << "  Price (USD)  |  Amount (BTC)  \n";
    for (const auto& ask : result["asks"]) {
        cout << "  " << ask[0] << "      |  " << ask[1] << "\n";
    }

    cout << "Bids\n";
    cout << "  Price (USD)  |  Amount (BTC)  \n";
    for (const auto& bid : result["bids"]) {
        cout << "  " << bid[0] << "      |  " << bid[1] << "\n";
    }
}

json get_positions_request() {
    string currency;
    cout << "Enter the currency (e.g., BTC, ETH): ";
    cin >> currency;

    json request = {
        {"jsonrpc", "2.0"},
        {"id", 1234},
        {"method", "private/get_positions"},
        {"params", {
            {"currency", currency}
        }}
    };

    return request;
}

json subscribe(){
    cout << "Enter the instrument name: ";
    string instrument;
    cin >> instrument;
    json sub = {
        {"jsonrpc", "2.0"},
        {"id", 1}, 
        {"method", "public/subscribe"},
        {"params", {
            {"channels", { "book." + instrument + ".100ms" }}
        }}
    };
    return sub;
}

void displayMenu() {
    cout << "\nSelect an option:" << endl;
    cout << "1. Place Order" << endl;
    cout << "2. Cancel Order" << endl;
    cout << "3. Modify Order" << endl;
    cout << "4. Get Order Book" << endl;
    cout << "5. Get Positions" << endl;
    cout << "6. Subscribe" << endl;
    cout << "Enter your choice: ";
}