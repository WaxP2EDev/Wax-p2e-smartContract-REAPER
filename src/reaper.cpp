#include <stdint.h>
#include <reaper.hpp>

using namespace eosio;
static inline time_point_sec current_time_point_sec() { return time_point_sec(current_time_point()); };

void reaper::regstaker(
    name from,
    name to,
    vector<uint64_t> asset_ids,
    string memo)
{
  if (from == get_self() || to != get_self())
    return;
  else
  {
    check(memo == "transfer", "Incorrect Memo !");
    auto it_ban = ban.find(from.value);
    check(it_ban == ban.end(), "You where banned, please see your administrator");

    auto itr = stakes.find(from.value);
    if (itr == stakes.end())
    {
      stakes.emplace(get_self(), [&](auto &v)
                     {
          v.username = from;
          v.last_updated = current_time_point_sec().utc_seconds;
          v.nfts = asset_ids;
          v.next_run = current_time_point_sec().utc_seconds + period;
          v.isstaked = true; });
    }
    else
    {
      ////// have to call FT or NFT claim functions ////////
    }
  }
}
ACTION reaper::banstaker(const name username)
{
  /*require_auth(username);
  auto itr = _staker_list.find(username.value);
  check(itr != _staker_list.end(), "That user is not a staker");
  _banned_list.emplace(get_self(), [&](auto &row)
                       {
    row.username = username;
      _staker_list.erase(itr); });*/
}

ACTION reaper::unstake(const name username, string memo)
{
  check(has_auth(username), "No authority");
  auto itr = stakes.require_find(username.value, "Your are not a staker, please register first");
  in_contract_transfer(username, itr->nfts, string("Your NFT_staked have been unstaked"));
  stakes.erase(itr);
}

ACTION reaper::burn(const name username, uint64_t assets_id, string memo)
{
  /*require_auth(username);
  check(memo == "burn", "plz insert transaction name");
  check(memo.size() > 256, "wrong message");

  action(
      permission_level{get_self(), "active"_n},
      "atomicassets"_n,
      "burnasset"_n,
      std::make_tuple(username, assets_id))
      .send();*/
}

void reaper::in_contract_transfer(name recipient, vector<uint64_t> assets_id, string msg)
{
  auto memo = " withdrawn by " + recipient.to_string();
  action(permission_level{get_self(), name("active")},
         "atomicassets"_n, name("transfer"),
         std::make_tuple(get_self(), recipient, assets_id, memo))
      .send();
}

ACTION reaper::claim(name username, string memo)
{
  require_auth(username);
  for (auto &item : machines)
  {
    for (uint8_t i = 0; i < item.productionMachine.size(); i++)
    {
      if (item.productionMachine[i].next_run < current_time_point_sec().utc_seconds)
      {
        machines.modify(item, _self, [&](auto &a)
                        {
        a.productionMachine[i].last_updated = a.productionMachine[i].next_run;
        a.productionMachine[i].next_run = a.productionMachine[i].last_updated + a.productionMachine[i].periodTime;
        ATTRIBUTE_MAP immutable_data = [];
        ATTRIBUTE_MAP mutable_data = [];
        vector<asset> tokens_to_back = [];
        rewardNFT(name("authorized_minter"), a.productionMachine[i].collection_name, a.productionMachine[i].schema_name, a.productionMachine[i].template_id, username, immutable_data, mutable_data, tokens_to_back); });
      }
    }
  }
}
ACTION reaper::regmachine(const name username, rewardmachine productMachine, string memo)
{
  require_auth(username);
  auto itr = machines.find(username.value);
  if (itr == machines.end())
  {
    itr = machines.emplace(username, [&](auto &row)
                           {
      row.username = username;
      rewardmachine updateMachine;
      updateMachine = productMachine;
      updateMachine.last_updated = current_time_point_sec().utc_seconds;
      updateMachine.next_run = updateMachine.last_updated + productMachine.production_time;
      row.productionMachine.push_back(updateMachine); });
  }
  else
  {
    uint8_t count = 0;
    if (productMachine.machine == "foundreis")
    {
      for (int i = 0; i < itr->productionMachine.size(); i++)
      {
        if (itr->productionMachine[i].machine == "foundreis")
        {
          count++;
          check(count == 4, "You can no longer create foundries.");
        }
      }
    }
    else if (productMachine.machine == "oil")
    {
      for (int i = 0; i < itr->productionMachine.size(); i++)
      {
        if (itr->productionMachine[i].machine == "oil")
        {
          count++;
          check(count == 5, "You can no longer create oil pump.");
        }
      }
    }
    machines.modify(itr, _self, [&](auto &a)
                    {
      rewardmachine updateMachine;
      updateMachine = productMachine;
      updateMachine.last_updated = current_time_point_sec().utc_seconds;
      updateMachine.next_run = updateMachine.last_updated + productMachine.production_time;
      a.productionMachine.push_back(updateMachine); });
  }
}
void reaper::rewardNFT(
    name authorized_minter,
    name collection_name,
    name schema_name,
    int32_t template_id,
    name new_asset_owner,
    ATTRIBUTE_MAP immutable_data,
    ATTRIBUTE_MAP mutable_data,
    vector<asset> tokens_to_back)
{
  require_auth(new_asset_owner);
  action(
      permission_level{get_self(), "active"_n},
      "atomicassets"_n,
      "mintasset"_n,
      std::make_tuple(get_self(), collection_name, schema_name, template_id, new_asset_owner, immutable_data, mutable_data, tokens_to_back))
      .send();
}
