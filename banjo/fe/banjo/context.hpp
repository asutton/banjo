// Copyright (c) 2015-2016 Andrew Sutton
// All rights reserved

#ifndef BANJO_FE_CONTEXT_HPP
#define BANJO_FE_CONTEXT_HPP

#include <banjo/context.hpp>


namespace banjo
{

namespace fe
{

// The compiler context defines information and facilities related to the
// translation of banjo syntax into core language constructs. This includes
// frontend initialization, maintaining compiler options, etc.
struct Context : banjo::Context
{
  Context();
};


} // namespace fe

} // namespace banjo


#endif
