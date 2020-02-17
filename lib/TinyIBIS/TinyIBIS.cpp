#include "TinyIBIS.h"

namespace TinyIBIS {
    static const char TAG[] = __FILE__;

    static constexpr ctll::fixed_string DS001_re = ctll::fixed_string{"l([0-9]{3})"};
    static constexpr ctll::fixed_string DS003_re = ctll::fixed_string{"^hP([0-9]{4})$"};
    static constexpr ctll::fixed_string DS010E_re = ctll::fixed_string{"^xV(/-|/+[0-9]{3})$"};

    std::optional<tMatch> MatchDS001(std::string_view sv) noexcept {
        if (auto m = ctre::match<DS001_re>(sv)) {
            return tMatch{t_telegram::DS001, m};
        };
        return std::nullopt;
    };

    std::optional<tMatch> MatchDS003(std::string_view sv) noexcept {
        if (auto m = ctre::match<DS003_re>(sv)) {
            return tMatch{t_telegram::DS003, m};
        }
        return std::nullopt;
    };

    std::optional<tMatch> MatchDS010E(std::string_view sv) noexcept {
        if (auto m = ctre::match<DS010E_re>(sv)) {
            return tMatch{t_telegram::DS010E, m};
        }
        return std::nullopt;
    };


    static const t_fmatch MatchFns[3] = {
        &MatchDS001,
        &MatchDS003,
        &MatchDS010E
    };

    void ParseDS001(ctre::regex_results<const char *, ctre::captured_content<1U>> m, DS001 &ds001) noexcept {
        ds001.updatedAt = now();
        ds001.valid = true,
        ds001.value = std::stoi(m.get<1>().to_string());
    };

    void ParseDS003(ctre::regex_results<const char *, ctre::captured_content<1U>> m, DS003 &ds003) noexcept {
        ds003.updatedAt = now();
        ds003.valid = true,
        ds003.value = std::stoi(m.get<1>().to_string());
    };

    void ParseDS010E(ctre::regex_results<const char *, ctre::captured_content<1U>> m, DS010E &ds010E) noexcept {
        ds010E.updatedAt = now();
        ds010E.valid = true,
        ds010E.value = std::stoi(m.get<1>().to_string());
    };



    Ibis::Ibis(/* args */)
        : parity(0), isParity(false), curTermOffset(0) {
        parity = PARITY_SEED;
        buffer[0] = 0;
        this->ds001 = {
            now(), // updatedAt
            false, // valid
            0      // value
        };
        this->ds003 = {
            now(), // updatedAt
            false, // valid
            0      // value
        };

        this->ds010e = {
            now(), // updatedAt
            false, // valid
            0      // value
        };
    };

    t_telegram Ibis::encode(char c) {
        if (curTermOffset > IBIS_MAX_FIELD_SIZE && !isParity) {
            // Buffer "overflow" - reset and return
            reset();
            return t_telegram::UKNOWN;
        }

        if (isParity) { // A parity byte follows a terminator
            if (parity == (uint8_t)c) {
                ESP_LOGV(TAG,"\nGet telegram type for: %s\n", buffer);
                const std::string_view sv = std::string_view{buffer};
                auto tt = GetTelegramType(sv);
                if (tt) {
                    switch (tt.value().type) {
                        case TinyIBIS::t_telegram::DS001:
                            ParseDS001(tt.value().m, this->ds001);
                            break;
                        case TinyIBIS::t_telegram::DS003:
                            ParseDS003(tt.value().m, this->ds003);
                            break;
                        case TinyIBIS::t_telegram::DS010E:
                            ParseDS010E(tt.value().m, this->ds010e);
                            break;
                        default:
                            break;
                    }
                    return tt.value().type;
                }
                reset();
                return t_telegram::UKNOWN;
            }
            reset();
            return t_telegram::UKNOWN;
            // Telegram END
        }
        parity ^= (uint8_t)c;
        if (c == _TERMINATOR) { // We use a terminator to sync
            isParity = true;
            buffer[curTermOffset] = '\0'; // End String
            return t_telegram::UKNOWN;
        }
        buffer[curTermOffset] = c;
        curTermOffset += 1;
        return t_telegram::UKNOWN;
    }

    void Ibis::reset() {
        curTermOffset = 0;
        parity = PARITY_SEED;
        isParity = false;
        isStart = false;
        memset(buffer, '\0', IBIS_MAX_FIELD_SIZE);
    }

    std::optional<tMatch> GetTelegramType(std::string_view sv) {
        const size_t se = sizeof(MatchFns) / sizeof(MatchFns[0]);
        for (int i = 0; i < se; i++) {
            auto result = MatchFns[i](sv);
            if (result) {
                return result;
            };
        }
        return std::nullopt;
    }

} // namespace TinyIBIS
