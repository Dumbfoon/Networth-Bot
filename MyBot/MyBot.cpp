#include "MyBot.h"
#include <dpp/dpp.h>

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

            bot.global_command_create(newcommand);
        }
    });

    bot.start(dpp::st_wait);

    return 0;
}
