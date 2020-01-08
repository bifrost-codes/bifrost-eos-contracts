#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

#include <string>

namespace bifrost {

   using std::string;
   using namespace eosio;

   class [[eosio::contract("bifrost.bridge")]] bridge : public contract {
   public:
      using contract::contract;

      bridge(name s, name code, datastream<const char *> ds);

      ~bridge();

      [[eosio::on_notify("eosio.token::transfer")]]
      void deposit_notify(const name &from,
                          const name &to,
                          const asset &quantity,
                          const string &memo);

      [[eosio::action]]
      void depositcnfm(uint64_t deposit_id);

      [[eosio::action]]
      void depositrlbk(uint64_t deposit_id);

      [[eosio::action]]
      void withdraw(const name &contract,
                    const name &to,
                    const asset &quantity,
                    const string &memo);

      [[eosio::action]]
      void activate();

      [[eosio::action]]
      void deactivate();

      using withdraw_action = eosio::action_wrapper<"withdraw"_n, &bridge::withdraw>;

   private:
      static constexpr eosio::name active_permission{"active"_n};
      static constexpr eosio::name eosio_token_contract{"eosio.token"_n};
      static constexpr eosio::name transfer_action{"transfer"_n};

      struct [[eosio::table]] globalstate {
         globalstate() {}

         uint64_t deposit_id = 1;
         bool active = true;

         EOSLIB_SERIALIZE( globalstate, (deposit_id)(active) )
      };

      struct [[eosio::table]] deposit {
         uint64_t id;
         name contract;
         name from;
         asset quantity;
         string memo;
         uint8_t status;

         EOSLIB_SERIALIZE( deposit, (id)(contract)(from)(quantity)(memo)(status) )

         uint64_t primary_key() const { return id; }
      };

      eosio::singleton<"globalstate"_n, globalstate> _global_state;
      globalstate _gstate;

      typedef eosio::multi_index<"deposits"_n, deposit> deposits;
   };

}
