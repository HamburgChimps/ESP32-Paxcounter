#include "TinyIBIS.h"

namespace TinyIBIS {
    static const char TAG[] = __FILE__;

    static constexpr ctll::fixed_string DS001_re = ctll::fixed_string{"^l([0-9]{3})$"};
    // static constexpr ctll::fixed_string DS003_re = ctll::fixed_string{"^z([0-9]{3})$"};
    static constexpr ctll::fixed_string DS036_re = ctll::fixed_string{"^hP([0-9]{4})$"};
    // static constexpr ctll::fixed_string DS010E_re = ctll::fixed_string{"^xV(/-|/+[0-9]{3})$"};

    std::optional<tMatch> MatchDS001(std::string_view sv) noexcept {
        if (auto m = ctre::match<DS001_re>(sv)) {
            return tMatch{t_telegram::DS001, m};
        };
        return std::nullopt;
    };

    // std::optional<tMatch> MatchDS003(std::string_view sv) noexcept {
    //     if (auto m = ctre::match<DS003_re>(sv)) {
    //         return tMatch{t_telegram::DS003, m}; 
    //     };
    //     return std::nullopt;
    // };

    std::optional<tMatch> MatchDS036(std::string_view sv) noexcept {
        if (auto m = ctre::match<DS036_re>(sv)) {
            return tMatch{t_telegram::DS036, m};
        }
        return std::nullopt;
    };

    // std::optional<tMatch> MatchDS010E(std::string_view sv) noexcept {
    //     if (auto m = ctre::match<DS010E_re>(sv)) {
    //         return tMatch{t_telegram::DS010E, m};
    //     }
    //     return std::nullopt;
    // };


    static const t_fmatch MatchFns[4] = {
        &MatchDS001,
        // &MatchDS003,
        &MatchDS036,
        // &MatchDS010E
    };

    Ibis::Ibis(/* args */)
        : parity(0), isParity(false), curTermOffset(0) {
        parity = PARITY_SEED;
        buffer[0] = 0;
    };

    t_telegram Ibis::encode(char c) {
        if (curTermOffset > IBIS_MAX_FIELD_SIZE && !isParity) {
            // Buffer "overflow" - reset and return
            reset();
            return t_telegram::UKNOWN;
        }

        if (isParity) { // A parity byte follows a terminator
            if (parity == (uint8_t)c) {
                ESP_LOGV(TAG,"Get telegram type for: %s\n", buffer);
                const std::string_view sv = std::string_view{buffer};
                auto tt = GetTelegramType(sv);
                if (tt) {
                    switch (tt.value().type) {
                        case TinyIBIS::t_telegram::DS001:
                            this->ParseDS001(tt.value().m);
                            break;
                        // case TinyIBIS::t_telegram::DS003:
                        //     this->ParseDS003(tt.value().m);
                        //     break;
                        case TinyIBIS::t_telegram::DS036:
                            this->ParseDS036(tt.value().m);
                            break;
                        // case TinyIBIS::t_telegram::DS010E:
                        //     this->ParseDS010E(tt.value().m);
                        //     break;
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

    void Ibis::ParseDS001(t_sResult m) noexcept {
        const auto s = m.get<1>().to_string();
        this->DS001.value = std::stoi(s);
        this->DS001.updatedAt = now();
        this->DS001.valid = true;
        ESP_LOGD(TAG, "Bus Line Number %s", s.c_str());
    };

    // void Ibis::ParseDS003(t_sResult m) noexcept {
    //     const auto s = m.get<1>().to_string();
    //     this->DS003.value = std::stoi(s);
    //     this->DS003.updatedAt = now();
    //     this->DS003.valid = true;
    //     ESP_LOGD(TAG, "Destination Number %s", s.c_str());
    // };

    void Ibis::ParseDS036(t_sResult m) noexcept {
        const auto s = m.get<1>().to_string();
        this->DS036.value = std::stoi(s);
        this->DS036.updatedAt = now();
        this->DS036.valid = true;
        ESP_LOGD(TAG, "Next Stop %s", s.c_str());
    };

    // void Ibis::ParseDS010E(t_sResult m) noexcept {
    //     const auto s = m.get<1>().to_string();
    //     this->DS010E.value = std::stoi(s);
    //     this->DS010E.updatedAt = now();
    //     this->DS010E.valid = true;
    //     ESP_LOGD(TAG, "Current Delay %s", s.c_str());
    // };

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
