#ifndef _TinyIBIS_h
#ifdef __cplusplus
#define _TinyIBIS_h
#define _TINYIBIS_VERSION 0.1
#define PARITY_SEED 0x7F
#define _TERMINATOR 0x0D
#include "Arduino.h"
#include "ctre.hpp"
#include <optional>
#include "microTime.h"



namespace TinyIBIS {
    const size_t IBIS_MAX_FIELD_SIZE = 50;

    enum class t_telegram {
        UKNOWN,
        DS001,
        DS003,
        DS036,
        DS010E
    };

    typedef ctre::regex_results<const char *, ctre::captured_content<1U>> t_sResult;

    struct tMatch {
      t_telegram type;
      t_sResult m;
    };

    typedef std::optional<tMatch> (*t_fmatch) (std::string_view) noexcept;
    
    std::optional<tMatch> GetTelegramType(std::string_view sv);

    class CommonTelegram {
      public:
        bool valid = false;
        time_t updatedAt = 0;
        time_t age() const    { return valid ? now() - updatedAt : (time_t)LONG_MAX; }
    };

    class DS001Telegram : public CommonTelegram {
      public:
        uint16_t value = 0;
    };


    class DS003Telegram : public CommonTelegram {
      public:
        uint16_t value = 0;
    };

    class DS036Telegram : public CommonTelegram {
      public:
        uint16_t value = 0;
    };

    class DS010ETelegram : public CommonTelegram {
      public:
        int16_t value = 0;
    };

    class Ibis {
      private:
        // parsing state variables
        uint8_t parity = PARITY_SEED;
        bool isParity = false;
        bool isStart = false;
        uint8_t curTermOffset = 0;
        char buffer[IBIS_MAX_FIELD_SIZE] = {0};
        void ParseDS001(t_sResult m) noexcept;
        void ParseDS003(t_sResult m) noexcept;
        void ParseDS036(t_sResult m) noexcept;
        void ParseDS010E(t_sResult m) noexcept;
        void reset();
      public:
        Ibis();
        t_telegram encode(char c);
        time_t updatedAt = now();
        DS001Telegram DS001 = DS001Telegram();
        DS003Telegram DS003 = DS003Telegram();
        DS036Telegram DS036 = DS036Telegram();
        DS010ETelegram DS010E = DS010ETelegram();
    };



} // namespace TinyIBIS
#endif // __cplusplus
#endif /* _TinyIbis_h */
