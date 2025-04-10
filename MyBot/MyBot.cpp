#include "MyBot.h"

#include <curl/curl.h>
#include <dpp/dpp.h>
#include <nlohmann/json.hpp>

#include <urlmon.h>
#include <windows.h>
#include <iostream>
#include <memory>
#include <string>

#pragma comment(lib, "urlmon.lib")

template <typename BasicJsonType>
std::string to_string(const BasicJsonType& j)
{
    return j.dump();
}

struct StreamDeleter {
    void operator()(IStream* p) const { if (p) p->Release(); }
};

std::string DownloadUrlContent(const std::wstring& url) {
    std::unique_ptr<IStream, StreamDeleter> stream;
    IStream* tempStream = nullptr;

    if (SUCCEEDED(URLOpenBlockingStream(nullptr, url.c_str(), &tempStream, 0, nullptr))) {
        stream.reset(tempStream);

        std::string content;
        char buffer[4096];
        ULONG bytesRead = 0;

        while (SUCCEEDED(stream->Read(buffer, sizeof(buffer), &bytesRead)) && bytesRead > 0) {
            content.append(buffer, bytesRead);
        }

        return content;
    }

    return "";
}

const std::string BOT_TOKEN = "<enter token here>";

int main()
{
    dpp::cluster bot(BOT_TOKEN);

    bot.on_log(dpp::utility::cout_logger());

    
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event)
    {
        
        if (event.command.get_command_name() == "networth") 
        {
            std::string username = std::get<std::string>(event.get_parameter("username"));
            event.reply(username + " is the goat.");
        }
        if (event.command.get_command_name() == "bazaar")
        {
            std::cout << "Input recognised as bazaar";
            std::string input = std::get<std::string>(event.get_parameter("product_id"));
            nlohmann::json info = nlohmann::json::parse(DownloadUrlContent(L"https://api.hypixel.net/skyblock/bazaar"))["products"][input]["quick_status"];
            std::cout << "Info buyprice is: " << info["buyPrice"] << std::endl;
            std::cout << "Info sellprice is: " << info["sellPrice"] << std::endl;
            event.reply(
                "Instabuy price is: " + to_string(info["buyPrice"]) + "\n" + 
                "Instasell price is: " + to_string(info["sellPrice"])
            );
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) 
    {
        if (dpp::run_once<struct register_bot_commands>()) 
        {
            dpp::slashcommand newcommand("networth", "Get networth of someone", bot.me.id);
            newcommand.add_option(
                dpp::command_option(dpp::co_string, "username", "IGN of person", true)
            );
            dpp::slashcommand command2("bazaar", "Get buy/sell price of item", bot.me.id);
            command2.add_option(
                dpp::command_option(dpp::co_string, "product_id", "Product ID according to skyblock", true)
            );
            bot.global_command_create(newcommand);
            bot.global_command_create(command2);
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}
