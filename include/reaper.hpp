#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/transaction.hpp>
#include <atomicassets.hpp>
#include <atomicdata.hpp>
#include <stdint.h>
#include <string>

using namespace eosio;
using namespace std;

CONTRACT reaper : public contract {
  public:
      using contract::contract;
      const uint32_t period = 6 * 60 * 60;
      struct imeta
      {
        name username;
        string collection_name;
        uint64_t template_id;
        uint64_t assets_id;
      };

      struct rewardmachine
      {
        string machine;
        uint64_t production_time;
        uint64_t last_updated;
        uint64_t next_run;
        uint64_t periodTime;
      };

      ACTION banstaker(const name username);
      ACTION unstake(const name username, string memo);
      ACTION claim(const name username, string memo);
      ACTION burn(const name username, uint64_t assets_id, string memo);
      ACTION regmachine(const name username, rewardmachine productMachine, string memo);

      [[eosio::on_notify("atomicassets::transfer")]] void regstaker(
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo
      );

  private:
    TABLE staker_t
    {
      name username; // name of the staking user
      vector<uint64_t> nfts;
      uint64_t last_updated;
      uint64_t next_run;
      bool isstaked; // if the users has already staked funds or not
      auto primary_key() const{ return username.value;}
    };

    TABLE machine_t
    {
      name username; // name of the staking user
      vector<rewardmachine> productionMachine;
      auto primary_key() const{ return username.value; }
    };

    TABLE ban_t
    {
      name username;
      asset fund_held; // funds that were staked by the user
      auto primary_key() const{ return username.value; }
    };

    TABLE admin_t
    {
      name username;
      auto primary_key() const{ return username.value; }
    };

    typedef eosio::multi_index<name("stakers"), staker_t> staker_s;
    typedef eosio::multi_index<name("machinelist"), machine_t> machine_s;
    typedef eosio::multi_index<name("bannedlist"), ban_t> ban_s;
    typedef eosio::multi_index<name("adminlist"), admin_t> admin_s;

    staker_s stakes = staker_s(get_self(), get_self().value);
    machine_s machines = machine_s(get_self(), get_self().value);
    ban_s ban = ban_s(get_self(), get_self().value);
    admin_s admin = admin_s(get_self(), get_self().value);

    void in_contract_transfer(name recipient, vector<uint64_t> assets_id, string msg);
    
    void rewardNFT(
        name authorized_minter,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name new_asset_owner,
        ATTRIBUTE_MAP immutable_data,
        ATTRIBUTE_MAP mutable_data,
        vector<asset> tokens_to_back);
};