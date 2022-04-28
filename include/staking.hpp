#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
// #include <../../../atomicassets-contract/include/atomicassets.hpp>
#include <map>
#include <string>
#include <iterator>
#include <vector>
#include <math.h>
// #include <totalstake.hpp>
using namespace eosio;
using namespace std;

using std::string;
using std::vector;

typedef uint128_t uuid;
typedef uint64_t id_type;
typedef string uri_type;
typedef std::variant<
    int8_t, int16_t, int32_t, int64_t,
    uint8_t, uint16_t, uint32_t, uint64_t,
    float, double, std::string,
    std::vector<int8_t>, std::vector<int16_t>, std::vector<int32_t>, std::vector<int64_t>,
    std::vector<uint8_t>, std::vector<uint16_t>, std::vector<uint32_t>, std::vector<uint64_t>,
    std::vector<float>, std::vector<double>, std::vector<std::string>>
    ATOMIC_ATTRIBUTE;
typedef std::map<std::string, ATOMIC_ATTRIBUTE> ATTRIBUTE_MAP;
CONTRACT reaper : public eosio::contract
{
public:
  using contract::contract;
  reaper(eosio::name receiver, eosio::name code, eosio::datastream<const char *> ds) : contract(receiver, code, ds),
                                                                                       _staker_list(receiver, receiver.value),
                                                                                       _banned_list(receiver, receiver.value),
                                                                                       _admin_list(receiver, receiver.value),

  {
  } // thre precision of the symbol is the decimal precision of

  const uint32_t period = 6 * 60 * 60;
  struct imeta
  {
    name username;
    string collection_name;
    id_type template_id;
    id_type assets_id;
  };
  struct rewardmachine
  {
    string machine;
    string collection_name;
    string schema_name;
    id_type template_id;
    string rarity;
    uint64_t production_time;
    time_point_sec last_updated;
    time_point_sec next_run;
    uint64_t periodTime;
  };

  ACTION regstaker(name username, vector<imeta> nftid_staked, string msg);

  ACTION banstaker(name username);

  ACTION addadmin(name username);

  ACTION unstake(name username, id_type assets_id);

  ACTION claim(name username, string memo);

  ACTION burn(name username, id_type assets_id, string memo);

private:
  TABLE staker
  {
    name username; // name of the staking user
    vector<imeta> nftid_staked;
    time_point_sec last_updated;
    time_point_sec next_run;
    bool isstaked; // if the users has already staked funds or not
    uint64_t primary_key() const { return username.value; }
  };
  typedef eosio::multi_index<name("stakerlist"), staker> stakers;
  stakers _staker_list;

  TABLE makeMachine
  {
    name username; // name of the staking user
    vector<rewardmachine> productionMachine;
    uint64_t primary_key() const { return username.value; }
  };
  typedef eosio::multi_index<name("makeMachineList"), makeMachine> makeMachines;
  makeMachines _makeMachine_list;

  TABLE banned
  {
    name username;
    asset fund_held; // funds that were staked by the user
    uint64_t primary_key() const { return username.value; }
  };
  typedef eosio::multi_index<name("bannedlist"), banned> banned_stakers;
  banned_stakers _banned_list;

  TABLE admin
  {
    name username;
    uint64_t primary_key() const { return username.value; }
  };
  typedef eosio::multi_index<name("adminlist"), admin> admins;
  admins _admin_list;

  TABLE account
  {

    asset balance;

    uint64_t primary_key() const { return balance.symbol.code().raw(); }
  };
  using account_index = eosio::multi_index<"accounts"_n, account>;

  void sub_balance(name owner, asset value);
  void add_balance(name owner, asset value, name ram_payer);
  void in_contract_transfer(name recipient, vector<id_type> assets_id, string msg);
  asset getReward(name username, string selectLand);
  void stake(name username, name receiver, vector<uint64_t> asset_ids, string msg);
  void transferNFT(name from, name to, id_type id, string memo);
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