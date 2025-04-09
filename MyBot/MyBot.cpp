#include "MyBot.h"
#include <curl/curl.h>
#include <dpp/dpp.h>
#include <urlmon.h>
#include <windows.h>
#include <iostream>
#include <memory>
#include <string>

#pragma comment(lib, "urlmon.lib")

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
    });

    bot.on_ready([&bot](const dpp::ready_t& event) 
    {
        if (dpp::run_once<struct register_bot_commands>()) {
            dpp::slashcommand newcommand("networth", "Get networth of someone", bot.me.id);
            newcommand.add_option(
                dpp::command_option(dpp::co_string, "username", "IGN of person", true)
            );
            dpp::slashcommand command2("viktor", "viktor rex", bot.me.id);

            bot.global_command_create(newcommand);
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}
