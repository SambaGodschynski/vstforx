/*
 * FrxAsyncDSPTimer.hpp
 *
 *  Created on: Fri Aug  9 11:00:46 2013
 *      Author: Johannes Unger
 */

#ifndef FRX_FRXASYNCDSPTIMER_H
#define FRX_FRXASYNCDSPTIMER_H

#include <com/FrxConfig.h>

#if FRX_ASYNC_DSP_TIMER == FRX_BOOST_TIMER
#include <sambag/com/BoostTimer2.hpp>
namespace frx { namespace processing {
    struct FrxAsyncDSPTimer : public sambag::com::BoostTimer2
    {
    };
}} // namespace(s)

#else
#include <sambag/disco/components/Timer.hpp>
#include <memory>
namespace frx { namespace processing {
    struct FrxAsyncDSPTimer : public sambag::disco::components::Timer
    {
        typedef std::shared_ptr<void> Dummy;
        typedef Dummy WorkerThreadHolder;
        static Dummy startWorkerThread() {
            return Dummy();
        }
        static void closeAllTimer() {}
    };
}} // namespace(s)

#endif // if FRX_ASYNC_DSP_TIMER == FRX_BOOST_TIMER



#endif /* FRX_FRXASYNCDSPTIMER_H */
