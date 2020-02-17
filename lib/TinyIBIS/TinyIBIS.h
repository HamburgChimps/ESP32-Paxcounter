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
        DS010E
    };

    struct common {
      time_t updatedAt = now();
      bool valid = false;
    };

    struct DS001 : common {
      uint16_t value = 0;
    };

    struct DS003 : common {
      uint16_t value = 0;
    };

    struct DS010E : common {
      int16_t value = 0;
    };

    struct tMatch {
      t_telegram type;
      ctre::regex_results<const char *, ctre::captured_content<1U>> m;
    };

    typedef std::optional<tMatch> (*t_fmatch) (std::string_view) noexcept;
    
    std::optional<tMatch> GetTelegramType(std::string_view sv);

    void ParseDS001(std::string_view sv, DS001 &ds001) noexcept;

    class Ibis {
      private:
        // parsing state variables
        uint8_t parity = PARITY_SEED;
        bool isParity = false;
        bool isStart = false;
        uint8_t curTermOffset = 0;
        char buffer[IBIS_MAX_FIELD_SIZE] = {0};
        void reset();
      public:
        Ibis();
        t_telegram encode(char c);
        DS001 ds001;
        DS003 ds003;
        DS010E ds010e;
    };



} // namespace TinyIBIS
#endif // __cplusplus
#endif /* _TinyIbis_h */
