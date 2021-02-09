#include "cryptonote_config.h"
#include "common/exp2.h"
#include "common/int-util.h"
#include <vector>
#include <boost/lexical_cast.hpp>

#include "service_node_rules.h"

namespace service_nodes
{
  uint64_t get_staking_requirement(cryptonote::network_type m_nettype, uint64_t height)
  {
    if(m_nettype != cryptonote::MAINNET)
      return arqma_bc::ARQMA * 1000;
    // We will need to work at some equation
    return arqma_bc::ARQMA * 10000;
  }

  uint64_t portions_to_amount(uint64_t portions, uint64_t staking_requirement)
  {
    uint64_t hi, lo, resulthi, resultlo;
    lo = mul128(staking_requirement, portions, &hi);
    div128_64(hi, lo, STAKING_SHARE_PARTS, &resulthi, &resultlo);
    return resultlo;
  }

  bool check_service_node_portions(const std::vector<uint64_t>& portions)
  {
    uint64_t portions_left = STAKING_SHARE_PARTS;

    for(const auto portion : portions)
    {
      const uint64_t min_portions = std::min(portions_left, STAKING_SHARE_PARTS);
      if(portion < min_portions || portion > portions_left)
        return false;
      portions_left -= portion;
    }

    return true;
  }

  uint64_t get_portions_to_make_amount(uint64_t staking_requirement, uint64_t amount)
  {
    uint64_t lo, hi, resulthi, resultlo;
    lo = mul128(amount, STAKING_SHARE_PARTS, &hi);
    if(lo > UINT64_MAX - (staking_requirement - 1))
      hi++;
    lo += staking_requirement-1;
    div128_64(hi, lo, staking_requirement, &resulthi, &resultlo);
    return resultlo;
  }

  static bool get_portions_from_percent(double cur_percent, uint64_t& portions)
  {
    if(cur_percent < 0.0 || cur_percent > 100.0) return false;

    // Fix for truncation issue when operator cut = 100 for a pool Service Node.
    if(cur_percent == 100.0)
    {
      portions = STAKING_SHARE_PARTS;
    }
    else
    {
      portions = (cur_percent / 100.0) * STAKING_SHARE_PARTS;
    }

    return true;
  }

  bool get_portions_from_percent_str(std::string cut_str, uint64_t& portions)
  {
    if(!cut_str.empty() && cut_str.back() == '%')
    {
      cut_str.pop_back();
    }

    double cut_percent;
    try
    {
      cut_percent = boost::lexical_cast<double>(cut_str);
    }
    catch(...)
    {
      return false;
    }

    return get_portions_from_percent(cut_percent, portions);
  }
}
