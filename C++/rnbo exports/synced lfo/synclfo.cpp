/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define trunc(x) ((Int)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class synclfo : public PatcherInterfaceImpl {
public:

synclfo()
{
}

~synclfo()
{
}

synclfo* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
    getEngine()->flushClockEvents(this, -1835586767, false);
    getEngine()->flushClockEvents(this, -131104204, false);
    getEngine()->flushClockEvents(this, 732390707, false);
    getEngine()->flushClockEvents(this, 1595885618, false);
    getEngine()->flushClockEvents(this, -972091856, false);
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number safepow(number base, number exponent) {
    return fixnan(rnbo_pow(base, exponent));
}

inline number safemod(number f, number m) {
    if (m != 0) {
        Int f_trunc = (Int)(trunc(f));
        Int m_trunc = (Int)(trunc(m));

        if (f == f_trunc && m == m_trunc) {
            f = f_trunc % m_trunc;
        } else {
            if (m < 0) {
                m = -m;
            }

            if (f >= m) {
                if (f >= m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f -= m;
                }
            } else if (f <= -m) {
                if (f <= -m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f += m;
                }
            }
        }
    } else {
        f = 0.0;
    }

    return f;
}

array<ListNum, 2> listcompare(const list& input1, const list& input2) {
    array<ListNum, 2> tmp = {};
    tmp[0] = 0;
    tmp[1] = {};
    list longList = (input1->length >= input2->length ? input1 : input2);
    list shortList = (input2->length <= input1->length ? input2 : input1);

    for (Index i = 0; i < shortList->length; i++) {
        if (longList[(Index)i] != shortList[(Index)i])
            tmp[1]->push(i);
    }

    for (Index i = (Index)(shortList->length); i < longList->length; i++) {
        tmp[1]->push(i);
    }

    if (tmp[1]->length > 0) {
        tmp[0] = 0;
    } else {
        tmp[0] = 1;
    }

    return tmp;
}

number random(number low, number high) {
    number range = high - low;
    return rand01() * range + low;
}

MillisecondTime currenttime() {
    return this->_currentTime;
}

number tempo() {
    return this->getTopLevelPatcher()->globaltransport_getTempo(this->currenttime());
}

number hztobeats(number hz) {
    return this->safediv(this->tempo() * 8, hz * 480.);
}

number samplerate() const {
    return this->sr;
}

number beattimeatsample(number offset) {
    return this->getTopLevelPatcher()->globaltransport_getBeatTimeAtSample(offset);
}

number transportatsample(SampleIndex sampleOffset) {
    return this->getTopLevelPatcher()->globaltransport_getStateAtSample(sampleOffset);
}

Index voice() {
    return this->_voiceIndex;
}

number tickstohz(number ticks) {
    return (number)1 / (ticks / (number)480 * this->safediv(60, this->tempo()));
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    RNBO_UNUSED(numInputs);
    RNBO_UNUSED(inputs);
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    SampleValue * out3 = (numOutputs >= 3 && outputs[2] ? outputs[2] : this->dummyBuffer);
    SampleValue * out4 = (numOutputs >= 4 && outputs[3] ? outputs[3] : this->dummyBuffer);
    SampleValue * out5 = (numOutputs >= 5 && outputs[4] ? outputs[4] : this->dummyBuffer);
    this->phasor_01_perform(this->phasor_01_freq, this->signals[0], n);
    this->subdivobject_01_perform(this->signals[0], this->signals[1], this->dummyBuffer, n);
    this->dspexpr_02_perform(this->signals[1], this->dspexpr_02_in2, this->signals[0], n);
    this->dspexpr_01_perform(this->signals[0], this->signals[1], n);

    this->scale_tilde_01_perform(
        this->signals[1],
        this->scale_tilde_01_lowin,
        this->scale_tilde_01_hiin,
        this->scale_tilde_01_lowout,
        this->scale_tilde_01_highout,
        this->scale_tilde_01_pow,
        out1,
        n
    );

    this->phasor_02_perform(this->phasor_02_freq, this->signals[1], n);
    this->subdivobject_02_perform(this->signals[1], out2, this->dummyBuffer, n);
    this->phasor_03_perform(this->phasor_03_freq, this->signals[1], n);
    this->subdivobject_03_perform(this->signals[1], this->signals[0], this->dummyBuffer, n);
    this->dspexpr_03_perform(this->signals[0], this->dspexpr_03_in2, out3, n);
    this->phasor_04_perform(this->phasor_04_freq, this->signals[0], n);
    this->subdivobject_04_perform(this->signals[0], this->signals[1], this->dummyBuffer, n);
    this->dspexpr_06_perform(this->signals[1], this->dspexpr_06_in2, this->signals[0], n);
    this->dspexpr_05_perform(this->signals[0], this->dspexpr_05_in2, this->signals[1], n);
    this->dspexpr_04_perform(this->signals[1], out4, n);
    this->phasor_05_perform(this->phasor_05_freq, this->signals[1], n);
    this->subdivobject_05_perform(this->signals[1], this->signals[0], this->dummyBuffer, n);
    this->delta_tilde_01_perform(this->signals[0], this->signals[1], n);
    this->dspexpr_07_perform(this->signals[1], this->dspexpr_07_in2, this->signals[0], n);
    this->noise_tilde_01_perform(this->signals[1], n);
    this->dspexpr_08_perform(this->signals[0], this->signals[1], this->signals[2], n);
    this->dspexpr_09_perform(this->signals[2], this->dspexpr_09_in2, this->signals[1], n);
    this->accum_tilde_01_perform(this->signals[1], this->accum_tilde_01_reset, this->signals[2], n);

    this->wrap_tilde_01_perform(
        this->signals[2],
        this->wrap_tilde_01_low,
        this->wrap_tilde_01_high,
        out5,
        n
    );

    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 3; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->phasor_01_sigbuf = resizeSignal(this->phasor_01_sigbuf, this->maxvs, maxBlockSize);
        this->phasor_02_sigbuf = resizeSignal(this->phasor_02_sigbuf, this->maxvs, maxBlockSize);
        this->phasor_03_sigbuf = resizeSignal(this->phasor_03_sigbuf, this->maxvs, maxBlockSize);
        this->phasor_04_sigbuf = resizeSignal(this->phasor_04_sigbuf, this->maxvs, maxBlockSize);
        this->phasor_05_sigbuf = resizeSignal(this->phasor_05_sigbuf, this->maxvs, maxBlockSize);
        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->phasor_01_dspsetup(forceDSPSetup);
    this->subdivobject_01_dspsetup(forceDSPSetup);
    this->phasor_02_dspsetup(forceDSPSetup);
    this->subdivobject_02_dspsetup(forceDSPSetup);
    this->phasor_03_dspsetup(forceDSPSetup);
    this->subdivobject_03_dspsetup(forceDSPSetup);
    this->phasor_04_dspsetup(forceDSPSetup);
    this->subdivobject_04_dspsetup(forceDSPSetup);
    this->phasor_05_dspsetup(forceDSPSetup);
    this->subdivobject_05_dspsetup(forceDSPSetup);
    this->delta_tilde_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 0;
}

void fillDataRef(DataRefIndex , DataRef& ) {}

void processDataViewUpdate(DataRefIndex , MillisecondTime ) {}

void initialize() {
    this->assign_defaults();
    this->setState();
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

void onSampleRateChanged(double samplerate) {
    this->timevalue_01_onSampleRateChanged(samplerate);
    this->timevalue_02_onSampleRateChanged(samplerate);
    this->timevalue_03_onSampleRateChanged(samplerate);
    this->timevalue_04_onSampleRateChanged(samplerate);
    this->timevalue_05_onSampleRateChanged(samplerate);
}

void processTempoEvent(MillisecondTime time, Tempo tempo) {
    this->updateTime(time);

    if (this->globaltransport_setTempo(this->_currentTime, tempo, false)) {
        this->timevalue_01_onTempoChanged(tempo);
        this->timevalue_02_onTempoChanged(tempo);
        this->timevalue_03_onTempoChanged(tempo);
        this->timevalue_04_onTempoChanged(tempo);
        this->timevalue_05_onTempoChanged(tempo);
    }
}

void processTransportEvent(MillisecondTime time, TransportState state) {
    this->updateTime(time);

    if (this->globaltransport_setState(this->_currentTime, state, false))
        {}
}

void processBeatTimeEvent(MillisecondTime time, BeatTime beattime) {
    this->updateTime(time);

    if (this->globaltransport_setBeatTime(this->_currentTime, beattime, false))
        {}
}

void processTimeSignatureEvent(MillisecondTime time, int numerator, int denominator) {
    this->updateTime(time);

    if (this->globaltransport_setTimeSignature(this->_currentTime, numerator, denominator, false)) {
        this->timevalue_01_onTimeSignatureChanged(numerator, denominator);
        this->timevalue_02_onTimeSignatureChanged(numerator, denominator);
        this->timevalue_03_onTimeSignatureChanged(numerator, denominator);
        this->timevalue_04_onTimeSignatureChanged(numerator, denominator);
        this->timevalue_05_onTimeSignatureChanged(numerator, denominator);
    }
}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "subdivide"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "subdivide"));
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 1;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "subdivide";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "subdivide";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 1;
            info->max = 128;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        {
            value = (value < 1 ? 1 : (value > 128 ? 128 : value));
            ParameterValue normalizedValue = (value - 1) / (128 - 1);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 0:
        {
        {
            {
                return 1 + value * (128 - 1);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void processClockEvent(MillisecondTime time, ClockId index, bool hasValue, ParameterValue value) {
    RNBO_UNUSED(hasValue);
    this->updateTime(time);

    switch (index) {
    case -1835586767:
        {
        this->subdivobject_01_stepmessage_set(value);
        break;
        }
    case -131104204:
        {
        this->subdivobject_02_stepmessage_set(value);
        break;
        }
    case 732390707:
        {
        this->subdivobject_03_stepmessage_set(value);
        break;
        }
    case 1595885618:
        {
        this->subdivobject_04_stepmessage_set(value);
        break;
        }
    case -972091856:
        {
        this->subdivobject_05_stepmessage_set(value);
        break;
        }
    }
}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {

    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->subdivobject_01_div_set(v);
    this->subdivobject_02_div_set(v);
    this->subdivobject_03_div_set(v);
    this->subdivobject_04_div_set(v);
    this->subdivobject_05_div_set(v);
}

void subdivobject_01_stepmessage_set(number v) {
    this->subdivobject_01_stepmessage = v;
}

void subdivobject_02_stepmessage_set(number v) {
    this->subdivobject_02_stepmessage = v;
}

void subdivobject_03_stepmessage_set(number v) {
    this->subdivobject_03_stepmessage = v;
}

void subdivobject_04_stepmessage_set(number v) {
    this->subdivobject_04_stepmessage = v;
}

void subdivobject_05_stepmessage_set(number v) {
    this->subdivobject_05_stepmessage = v;
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

Index getNumInputChannels() const {
    return 0;
}

Index getNumOutputChannels() const {
    return 5;
}

void allocateDataRefs() {}

void initializeObjects() {
    this->subdivobject_01_subdivider_init();
    this->subdivobject_02_subdivider_init();
    this->subdivobject_03_subdivider_init();
    this->subdivobject_04_subdivider_init();
    this->subdivobject_05_subdivider_init();
    this->noise_tilde_01_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());
    this->timevalue_01_sendValue();
    this->timevalue_02_sendValue();
    this->timevalue_03_sendValue();
    this->timevalue_04_sendValue();
    this->timevalue_05_sendValue();

    {
        this->scheduleParamInit(0, 0);
    }

    this->processParamInitEvents();
}

number param_01_value_constrain(number v) const {
    v = (v > 128 ? 128 : (v < 1 ? 1 : v));
    return v;
}

void subdivobject_01_div_set(number v) {
    this->subdivobject_01_div = v;
}

void subdivobject_02_div_set(number v) {
    this->subdivobject_02_div = v;
}

void subdivobject_03_div_set(number v) {
    this->subdivobject_03_div = v;
}

void subdivobject_04_div_set(number v) {
    this->subdivobject_04_div = v;
}

void subdivobject_05_div_set(number v) {
    this->subdivobject_05_div = v;
}

void phasor_01_freq_set(number v) {
    this->phasor_01_freq = v;
}

void timevalue_01_out_set(number v) {
    this->phasor_01_freq_set(v);
}

void phasor_02_freq_set(number v) {
    this->phasor_02_freq = v;
}

void timevalue_02_out_set(number v) {
    this->phasor_02_freq_set(v);
}

void phasor_03_freq_set(number v) {
    this->phasor_03_freq = v;
}

void timevalue_03_out_set(number v) {
    this->phasor_03_freq_set(v);
}

void phasor_04_freq_set(number v) {
    this->phasor_04_freq = v;
}

void timevalue_04_out_set(number v) {
    this->phasor_04_freq_set(v);
}

void phasor_05_freq_set(number v) {
    this->phasor_05_freq = v;
}

void timevalue_05_out_set(number v) {
    this->phasor_05_freq_set(v);
}

void phasor_01_perform(number freq, SampleValue * out, Index n) {
    auto __phasor_01_lastLockedPhase = this->phasor_01_lastLockedPhase;
    auto quantum = this->hztobeats(freq);
    auto tempo = this->tempo();

    if (quantum > 0 && tempo > 0) {
        auto samplerate = this->samplerate();
        auto beattime = this->beattimeatsample(0);
        number offset = fmod(beattime, quantum);
        number nextJump = quantum - offset;
        number nextJumpInSamples = rnbo_fround(nextJump * 60 * samplerate / tempo * 1 / (number)1) * 1;
        number inc = tempo / (number)60 / samplerate;
        number oneoverquantum = (number)1 / quantum;

        for (Index i = 0; i < n; i++) {
            if ((bool)(this->transportatsample(i))) {
                out[(Index)i] = __phasor_01_lastLockedPhase = offset * oneoverquantum;
                offset += inc;
                nextJumpInSamples--;

                if (nextJumpInSamples <= 0) {
                    offset -= quantum;
                    nextJumpInSamples = rnbo_fround(quantum * 60 * samplerate / tempo * 1 / (number)1) * 1;
                }
            } else {
                out[(Index)i] = __phasor_01_lastLockedPhase;
            }
        }
    } else {
        for (Index i = 0; i < n; i++) {
            out[(Index)i] = 0;
        }
    }

    this->phasor_01_lastLockedPhase = __phasor_01_lastLockedPhase;
}

void subdivobject_01_perform(
    const SampleValue * input_signal,
    SampleValue * output,
    SampleValue * step,
    Index n
) {
    auto __subdivobject_01_silentmode = this->subdivobject_01_silentmode;
    auto __subdivobject_01_lockprob = this->subdivobject_01_lockprob;
    auto __subdivobject_01_pattern_list = this->subdivobject_01_pattern_list;
    auto __subdivobject_01_prob_list = this->subdivobject_01_prob_list;
    auto __subdivobject_01_syncupdate = this->subdivobject_01_syncupdate;
    auto __subdivobject_01_div = this->subdivobject_01_div;
    number stepvalue = 0;

    for (Index i = 0; i < n; i++) {
        array<number, 2> result = this->subdivobject_01_subdivider_next(
            input_signal[(Index)i],
            __subdivobject_01_div,
            __subdivobject_01_syncupdate,
            __subdivobject_01_prob_list,
            __subdivobject_01_pattern_list,
            __subdivobject_01_lockprob,
            __subdivobject_01_silentmode
        );

        stepvalue = result[1];
        output[(Index)i] = result[0];
        step[(Index)i] = stepvalue;
    }

    this->getEngine()->scheduleClockEventWithValue(
        this,
        -1835586767,
        this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
        stepvalue
    );;
}

void dspexpr_02_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 6.28;//#map:_###_obj_###_:1
    }
}

void dspexpr_01_perform(const Sample * in1, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = rnbo_cos(in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void scale_tilde_01_perform(
    const Sample * x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow,
    SampleValue * out1,
    Index n
) {
    RNBO_UNUSED(pow);
    RNBO_UNUSED(highout);
    RNBO_UNUSED(lowout);
    RNBO_UNUSED(hiin);
    RNBO_UNUSED(lowin);
    auto inscale = this->safediv(1., 1 - -1);
    number outdiff = 1 - 0;
    Index i;

    for (i = 0; i < n; i++) {
        number value = (x[(Index)i] - -1) * inscale;
        value = value * outdiff + 0;
        out1[(Index)i] = value;
    }
}

void phasor_02_perform(number freq, SampleValue * out, Index n) {
    auto __phasor_02_lastLockedPhase = this->phasor_02_lastLockedPhase;
    auto quantum = this->hztobeats(freq);
    auto tempo = this->tempo();

    if (quantum > 0 && tempo > 0) {
        auto samplerate = this->samplerate();
        auto beattime = this->beattimeatsample(0);
        number offset = fmod(beattime, quantum);
        number nextJump = quantum - offset;
        number nextJumpInSamples = rnbo_fround(nextJump * 60 * samplerate / tempo * 1 / (number)1) * 1;
        number inc = tempo / (number)60 / samplerate;
        number oneoverquantum = (number)1 / quantum;

        for (Index i = 0; i < n; i++) {
            if ((bool)(this->transportatsample(i))) {
                out[(Index)i] = __phasor_02_lastLockedPhase = offset * oneoverquantum;
                offset += inc;
                nextJumpInSamples--;

                if (nextJumpInSamples <= 0) {
                    offset -= quantum;
                    nextJumpInSamples = rnbo_fround(quantum * 60 * samplerate / tempo * 1 / (number)1) * 1;
                }
            } else {
                out[(Index)i] = __phasor_02_lastLockedPhase;
            }
        }
    } else {
        for (Index i = 0; i < n; i++) {
            out[(Index)i] = 0;
        }
    }

    this->phasor_02_lastLockedPhase = __phasor_02_lastLockedPhase;
}

void subdivobject_02_perform(
    const SampleValue * input_signal,
    SampleValue * output,
    SampleValue * step,
    Index n
) {
    auto __subdivobject_02_silentmode = this->subdivobject_02_silentmode;
    auto __subdivobject_02_lockprob = this->subdivobject_02_lockprob;
    auto __subdivobject_02_pattern_list = this->subdivobject_02_pattern_list;
    auto __subdivobject_02_prob_list = this->subdivobject_02_prob_list;
    auto __subdivobject_02_syncupdate = this->subdivobject_02_syncupdate;
    auto __subdivobject_02_div = this->subdivobject_02_div;
    number stepvalue = 0;

    for (Index i = 0; i < n; i++) {
        array<number, 2> result = this->subdivobject_02_subdivider_next(
            input_signal[(Index)i],
            __subdivobject_02_div,
            __subdivobject_02_syncupdate,
            __subdivobject_02_prob_list,
            __subdivobject_02_pattern_list,
            __subdivobject_02_lockprob,
            __subdivobject_02_silentmode
        );

        stepvalue = result[1];
        output[(Index)i] = result[0];
        step[(Index)i] = stepvalue;
    }

    this->getEngine()->scheduleClockEventWithValue(
        this,
        -131104204,
        this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
        stepvalue
    );;
}

void phasor_03_perform(number freq, SampleValue * out, Index n) {
    auto __phasor_03_lastLockedPhase = this->phasor_03_lastLockedPhase;
    auto quantum = this->hztobeats(freq);
    auto tempo = this->tempo();

    if (quantum > 0 && tempo > 0) {
        auto samplerate = this->samplerate();
        auto beattime = this->beattimeatsample(0);
        number offset = fmod(beattime, quantum);
        number nextJump = quantum - offset;
        number nextJumpInSamples = rnbo_fround(nextJump * 60 * samplerate / tempo * 1 / (number)1) * 1;
        number inc = tempo / (number)60 / samplerate;
        number oneoverquantum = (number)1 / quantum;

        for (Index i = 0; i < n; i++) {
            if ((bool)(this->transportatsample(i))) {
                out[(Index)i] = __phasor_03_lastLockedPhase = offset * oneoverquantum;
                offset += inc;
                nextJumpInSamples--;

                if (nextJumpInSamples <= 0) {
                    offset -= quantum;
                    nextJumpInSamples = rnbo_fround(quantum * 60 * samplerate / tempo * 1 / (number)1) * 1;
                }
            } else {
                out[(Index)i] = __phasor_03_lastLockedPhase;
            }
        }
    } else {
        for (Index i = 0; i < n; i++) {
            out[(Index)i] = 0;
        }
    }

    this->phasor_03_lastLockedPhase = __phasor_03_lastLockedPhase;
}

void subdivobject_03_perform(
    const SampleValue * input_signal,
    SampleValue * output,
    SampleValue * step,
    Index n
) {
    auto __subdivobject_03_silentmode = this->subdivobject_03_silentmode;
    auto __subdivobject_03_lockprob = this->subdivobject_03_lockprob;
    auto __subdivobject_03_pattern_list = this->subdivobject_03_pattern_list;
    auto __subdivobject_03_prob_list = this->subdivobject_03_prob_list;
    auto __subdivobject_03_syncupdate = this->subdivobject_03_syncupdate;
    auto __subdivobject_03_div = this->subdivobject_03_div;
    number stepvalue = 0;

    for (Index i = 0; i < n; i++) {
        array<number, 2> result = this->subdivobject_03_subdivider_next(
            input_signal[(Index)i],
            __subdivobject_03_div,
            __subdivobject_03_syncupdate,
            __subdivobject_03_prob_list,
            __subdivobject_03_pattern_list,
            __subdivobject_03_lockprob,
            __subdivobject_03_silentmode
        );

        stepvalue = result[1];
        output[(Index)i] = result[0];
        step[(Index)i] = stepvalue;
    }

    this->getEngine()->scheduleClockEventWithValue(
        this,
        732390707,
        this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
        stepvalue
    );;
}

void dspexpr_03_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] >= 0.5;//#map:_###_obj_###_:1
    }
}

void phasor_04_perform(number freq, SampleValue * out, Index n) {
    auto __phasor_04_lastLockedPhase = this->phasor_04_lastLockedPhase;
    auto quantum = this->hztobeats(freq);
    auto tempo = this->tempo();

    if (quantum > 0 && tempo > 0) {
        auto samplerate = this->samplerate();
        auto beattime = this->beattimeatsample(0);
        number offset = fmod(beattime, quantum);
        number nextJump = quantum - offset;
        number nextJumpInSamples = rnbo_fround(nextJump * 60 * samplerate / tempo * 1 / (number)1) * 1;
        number inc = tempo / (number)60 / samplerate;
        number oneoverquantum = (number)1 / quantum;

        for (Index i = 0; i < n; i++) {
            if ((bool)(this->transportatsample(i))) {
                out[(Index)i] = __phasor_04_lastLockedPhase = offset * oneoverquantum;
                offset += inc;
                nextJumpInSamples--;

                if (nextJumpInSamples <= 0) {
                    offset -= quantum;
                    nextJumpInSamples = rnbo_fround(quantum * 60 * samplerate / tempo * 1 / (number)1) * 1;
                }
            } else {
                out[(Index)i] = __phasor_04_lastLockedPhase;
            }
        }
    } else {
        for (Index i = 0; i < n; i++) {
            out[(Index)i] = 0;
        }
    }

    this->phasor_04_lastLockedPhase = __phasor_04_lastLockedPhase;
}

void subdivobject_04_perform(
    const SampleValue * input_signal,
    SampleValue * output,
    SampleValue * step,
    Index n
) {
    auto __subdivobject_04_silentmode = this->subdivobject_04_silentmode;
    auto __subdivobject_04_lockprob = this->subdivobject_04_lockprob;
    auto __subdivobject_04_pattern_list = this->subdivobject_04_pattern_list;
    auto __subdivobject_04_prob_list = this->subdivobject_04_prob_list;
    auto __subdivobject_04_syncupdate = this->subdivobject_04_syncupdate;
    auto __subdivobject_04_div = this->subdivobject_04_div;
    number stepvalue = 0;

    for (Index i = 0; i < n; i++) {
        array<number, 2> result = this->subdivobject_04_subdivider_next(
            input_signal[(Index)i],
            __subdivobject_04_div,
            __subdivobject_04_syncupdate,
            __subdivobject_04_prob_list,
            __subdivobject_04_pattern_list,
            __subdivobject_04_lockprob,
            __subdivobject_04_silentmode
        );

        stepvalue = result[1];
        output[(Index)i] = result[0];
        step[(Index)i] = stepvalue;
    }

    this->getEngine()->scheduleClockEventWithValue(
        this,
        1595885618,
        this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
        stepvalue
    );;
}

void dspexpr_06_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 2;//#map:_###_obj_###_:1
    }
}

void dspexpr_05_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] - 1;//#map:_###_obj_###_:1
    }
}

void dspexpr_04_perform(const Sample * in1, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = rnbo_abs(in1[(Index)i]);//#map:_###_obj_###_:1
    }
}

void phasor_05_perform(number freq, SampleValue * out, Index n) {
    auto __phasor_05_lastLockedPhase = this->phasor_05_lastLockedPhase;
    auto quantum = this->hztobeats(freq);
    auto tempo = this->tempo();

    if (quantum > 0 && tempo > 0) {
        auto samplerate = this->samplerate();
        auto beattime = this->beattimeatsample(0);
        number offset = fmod(beattime, quantum);
        number nextJump = quantum - offset;
        number nextJumpInSamples = rnbo_fround(nextJump * 60 * samplerate / tempo * 1 / (number)1) * 1;
        number inc = tempo / (number)60 / samplerate;
        number oneoverquantum = (number)1 / quantum;

        for (Index i = 0; i < n; i++) {
            if ((bool)(this->transportatsample(i))) {
                out[(Index)i] = __phasor_05_lastLockedPhase = offset * oneoverquantum;
                offset += inc;
                nextJumpInSamples--;

                if (nextJumpInSamples <= 0) {
                    offset -= quantum;
                    nextJumpInSamples = rnbo_fround(quantum * 60 * samplerate / tempo * 1 / (number)1) * 1;
                }
            } else {
                out[(Index)i] = __phasor_05_lastLockedPhase;
            }
        }
    } else {
        for (Index i = 0; i < n; i++) {
            out[(Index)i] = 0;
        }
    }

    this->phasor_05_lastLockedPhase = __phasor_05_lastLockedPhase;
}

void subdivobject_05_perform(
    const SampleValue * input_signal,
    SampleValue * output,
    SampleValue * step,
    Index n
) {
    auto __subdivobject_05_silentmode = this->subdivobject_05_silentmode;
    auto __subdivobject_05_lockprob = this->subdivobject_05_lockprob;
    auto __subdivobject_05_pattern_list = this->subdivobject_05_pattern_list;
    auto __subdivobject_05_prob_list = this->subdivobject_05_prob_list;
    auto __subdivobject_05_syncupdate = this->subdivobject_05_syncupdate;
    auto __subdivobject_05_div = this->subdivobject_05_div;
    number stepvalue = 0;

    for (Index i = 0; i < n; i++) {
        array<number, 2> result = this->subdivobject_05_subdivider_next(
            input_signal[(Index)i],
            __subdivobject_05_div,
            __subdivobject_05_syncupdate,
            __subdivobject_05_prob_list,
            __subdivobject_05_pattern_list,
            __subdivobject_05_lockprob,
            __subdivobject_05_silentmode
        );

        stepvalue = result[1];
        output[(Index)i] = result[0];
        step[(Index)i] = stepvalue;
    }

    this->getEngine()->scheduleClockEventWithValue(
        this,
        -972091856,
        this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
        stepvalue
    );;
}

void delta_tilde_01_perform(const Sample * x, SampleValue * out1, Index n) {
    auto __delta_tilde_01_prev = this->delta_tilde_01_prev;
    Index i;

    for (i = 0; i < n; i++) {
        number temp = (number)(x[(Index)i] - __delta_tilde_01_prev);
        __delta_tilde_01_prev = x[(Index)i];
        out1[(Index)i] = temp;
    }

    this->delta_tilde_01_prev = __delta_tilde_01_prev;
}

void dspexpr_07_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] < 0;//#map:_###_obj_###_:1
    }
}

void noise_tilde_01_perform(SampleValue * out1, Index n) {
    auto __noise_tilde_01_state = this->noise_tilde_01_state;
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = xoshiro_next(__noise_tilde_01_state);
    }
}

void dspexpr_08_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void dspexpr_09_perform(const Sample * in1, number in2, SampleValue * out1, Index n) {
    RNBO_UNUSED(in2);
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * 0.05;//#map:_###_obj_###_:1
    }
}

void accum_tilde_01_perform(const Sample * x, number reset, SampleValue * out1, Index n) {
    RNBO_UNUSED(reset);
    auto __accum_tilde_01_value = this->accum_tilde_01_value;
    Index i;

    for (i = 0; i < n; i++) {
        __accum_tilde_01_value = __accum_tilde_01_value + x[(Index)i];
        out1[(Index)i] = __accum_tilde_01_value;
        continue;
    }

    this->accum_tilde_01_value = __accum_tilde_01_value;
}

void wrap_tilde_01_perform(const Sample * x, number low, number high, SampleValue * out1, Index n) {
    RNBO_UNUSED(high);
    RNBO_UNUSED(low);
    Index i;

    for (i = 0; i < n; i++) {
        number lo;
        number hi;
        lo = 0;
        hi = 1;
        number range = hi - lo;

        if (x[(Index)i] >= lo && x[(Index)i] < hi) {
            out1[(Index)i] = x[(Index)i];
            continue;
        }

        if (range <= 0.000000001) {
            out1[(Index)i] = lo;
            continue;
        }

        long numWraps = (long)(trunc((x[(Index)i] - lo) / range));
        numWraps = numWraps - ((x[(Index)i] < lo ? 1 : 0));
        number result = x[(Index)i] - range * numWraps;

        if (result >= hi) {
            out1[(Index)i] = result - range;
            continue;
        } else {
            out1[(Index)i] = result;
            continue;
        }
    }
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void subdivobject_01_subdivider_rebuild() {
    number i; number j; number c; number totaltime; number totalsteps;

    if (this->subdivobject_01_subdivider_pattern->length == 0) {
        this->subdivobject_01_subdivider_p2length = 0;
        this->subdivobject_01_subdivider_patternstep = 0;
        this->subdivobject_01_subdivider_patternphase = 0;
        return;
    }

    totaltime = 0;

    for (i = 0; i < this->subdivobject_01_subdivider_pattern->length; i++) {
        totaltime += this->subdivobject_01_subdivider_pattern[(Index)i];
    }

    i = this->safemod(totaltime, this->subdivobject_01_subdivider_div);
    totalsteps = (i == 0 ? totaltime : totaltime + (this->subdivobject_01_subdivider_div - i));

    if (this->subdivobject_01_subdivider_p2length != totalsteps) {
        this->subdivobject_01_subdivider_p2length = totalsteps;
    }

    this->subdivobject_01_subdivider_p2 = {};
    this->subdivobject_01_subdivider_synco = {};
    this->subdivobject_01_subdivider_p2prob = {};

    for (number i = 0; i < totalsteps; i++) {
        this->subdivobject_01_subdivider_p2->push(1);
        this->subdivobject_01_subdivider_synco->push(0);
        this->subdivobject_01_subdivider_p2prob->push(1.0);
    }

    totaltime = 0;
    c = 0;

    for (i = 0; i < this->subdivobject_01_subdivider_pattern->length; i++) {
        number nextvalue = (number)(this->subdivobject_01_subdivider_pattern[(Index)i]);
        number nextstart = (number)(this->safemod(totaltime, this->subdivobject_01_subdivider_div));
        number pv;

        if (this->subdivobject_01_subdivider_prob->length > 0) {
            number ps = (number)(this->safemod(i, this->subdivobject_01_subdivider_prob->length));
            pv = this->subdivobject_01_subdivider_prob[(Index)ps];
        } else {
            pv = 1.0;
        }

        for (j = 0; j < nextvalue; j++) {
            this->subdivobject_01_subdivider_p2[(Index)c] = nextvalue;

            if (nextvalue > 1)
                this->subdivobject_01_subdivider_synco[(Index)c] = nextstart;
            else
                this->subdivobject_01_subdivider_synco[(Index)c] = 0;

            if (j == 0)
                this->subdivobject_01_subdivider_p2prob[(Index)c] = pv;
            else
                this->subdivobject_01_subdivider_p2prob[(Index)c] = -1;

            c++;
        }

        totaltime += nextvalue;
    }

    for (j = 0; c < totalsteps; c++, j++) {
        number pv = 1.0;
        this->subdivobject_01_subdivider_p2[(Index)c] = totalsteps - totaltime;
        this->subdivobject_01_subdivider_synco[(Index)c] = this->safemod(totaltime, this->subdivobject_01_subdivider_div);

        if (j == 0) {
            if (this->subdivobject_01_subdivider_prob->length > 0) {
                number ps = (number)(this->safemod(i, this->subdivobject_01_subdivider_prob->length));
                pv = this->subdivobject_01_subdivider_prob[(Index)ps];
            } else {
                pv = 1.0;
            }
        } else {
            pv = -1.0;
        }

        this->subdivobject_01_subdivider_p2prob[(Index)c] = pv;
    }
}

void subdivobject_01_subdivider_reset() {
    this->subdivobject_01_subdivider_outstep = 0;
    this->subdivobject_01_subdivider_posstep = 0;
    this->subdivobject_01_subdivider_probstep = 0;
    this->subdivobject_01_subdivider_lastoutstep = 0;
    this->subdivobject_01_subdivider_div = 1;
    this->subdivobject_01_subdivider_nextdiv = 1;
    this->subdivobject_01_subdivider_resolveSync();
    this->subdivobject_01_subdivider_playing = this->subdivobject_01_subdivider_isPlaying(0);
    this->subdivobject_01_subdivider_patternstep = 0;
}

void subdivobject_01_subdivider_dspsetup() {
    this->subdivobject_01_subdivider_reset();
    this->subdivobject_01_subdivider_rebuild();
}

void subdivobject_01_subdivider_init() {
    this->subdivobject_01_subdivider_reset();
    this->subdivobject_01_subdivider_rebuild();
}

void subdivobject_01_subdivider_setProb(list prob) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_01_subdivider_prob, prob)[0]))) {
        this->subdivobject_01_subdivider_prob = {};

        if (prob->length > 0) {
            for (number i = 0; i < prob->length; i++) {
                this->subdivobject_01_subdivider_prob->push(
                    (prob[(Index)i] > 1.0 ? 1.0 : (prob[(Index)i] < 0.0 ? 0.0 : prob[(Index)i]))
                );
            }
        }

        this->subdivobject_01_subdivider_rebuild();
    }
}

void subdivobject_01_subdivider_setPattern(list pattern) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_01_subdivider_pattern, pattern)[0]))) {
        this->subdivobject_01_subdivider_pattern = {};

        if (pattern->length > 0) {
            for (number i = 0; i < pattern->length; i++) {
                this->subdivobject_01_subdivider_pattern->push(
                    (pattern[(Index)i] > this->subdivobject_01_subdivider_div ? this->subdivobject_01_subdivider_div : (pattern[(Index)i] < 1 ? 1 : pattern[(Index)i]))
                );
            }
        }

        this->subdivobject_01_subdivider_patternstep = 0;
        this->subdivobject_01_subdivider_rebuild();
    }
}

void subdivobject_01_subdivider_setDiv(int div) {
    if (div != this->subdivobject_01_subdivider_nextdiv) {
        this->subdivobject_01_subdivider_nextdiv = div;
    }
}

number subdivobject_01_subdivider_getPatternNumerator(number step) {
    number value = (number)(this->subdivobject_01_subdivider_p2[(Index)(this->subdivobject_01_subdivider_patternstep + step)]);
    this->subdivobject_01_subdivider_patternphase = this->subdivobject_01_subdivider_synco[(Index)(this->subdivobject_01_subdivider_patternstep + step)];

    if (step == this->subdivobject_01_subdivider_div - 1) {
        if (this->subdivobject_01_subdivider_patternstep + this->subdivobject_01_subdivider_div >= this->subdivobject_01_subdivider_p2length) {
            this->subdivobject_01_subdivider_patternstep = 0;
        } else {
            this->subdivobject_01_subdivider_patternstep += this->subdivobject_01_subdivider_div;
        }
    }

    return value;
}

bool subdivobject_01_subdivider_getPatternProbability(number step, bool playing) {
    number pv = (number)(this->subdivobject_01_subdivider_p2prob[(Index)(this->subdivobject_01_subdivider_patternstep + step)]);

    if (pv == -1) {
        return playing;
    }

    if (pv == 0) {
        return false;
    }

    if (pv == 1) {
        return true;
    }

    return this->random(0, 1) < pv;
}

bool subdivobject_01_subdivider_isPlaying(number step) {
    bool result = false;
    bool decided = false;
    number pv;

    if (this->subdivobject_01_subdivider_prob->length > 0 && (bool)(!(bool)(this->subdivobject_01_subdivider_lockprob))) {
        number ps = (number)(this->subdivobject_01_subdivider_probstep);

        if (ps >= this->subdivobject_01_subdivider_prob->length) {
            ps = 0;
        }

        pv = this->subdivobject_01_subdivider_prob[(Index)ps];
        result = this->random(0, 1) < pv;
        decided = true;
    }

    if (this->subdivobject_01_subdivider_pattern->length > 0) {
        if ((bool)(!(bool)(decided))) {
            result = this->subdivobject_01_subdivider_getPatternProbability(step, this->subdivobject_01_subdivider_playing);
        }

        this->subdivobject_01_subdivider_num = this->subdivobject_01_subdivider_getPatternNumerator(step);
    } else {
        this->subdivobject_01_subdivider_num = 1;

        if ((bool)(this->subdivobject_01_subdivider_prob->length) && (bool)(!(bool)(decided))) {
            number index = (number)(this->safemod(step, this->subdivobject_01_subdivider_prob->length));
            pv = this->subdivobject_01_subdivider_prob[(Index)index];

            if (pv == 0) {
                return false;
            }

            if (pv == 1) {
                return true;
            }

            return this->random(0, 1) < pv;
        } else if ((bool)(!(bool)(decided))) {
            result = true;
        }
    }

    return result;
}

bool subdivobject_01_subdivider_detectReset(number v, number p, number p2) {
    if (p2 < p && v < p && p - v > 0.25) {
        return true;
    }

    if (p2 > p && v > p && v - p > 0.25) {
        return true;
    }

    return false;
}

void subdivobject_01_subdivider_resolveSync() {
    if (this->subdivobject_01_subdivider_nextdiv != this->subdivobject_01_subdivider_div) {
        this->subdivobject_01_subdivider_div = this->subdivobject_01_subdivider_nextdiv;

        if (this->subdivobject_01_subdivider_pattern->length > 0) {
            this->subdivobject_01_subdivider_rebuild();
        }
    }
}

array<number, 2> subdivobject_01_subdivider_next(
    number x,
    number divarg,
    Int syncupdate,
    list prob,
    list pattern,
    bool lockprob,
    bool silentmode
) {
    this->subdivobject_01_subdivider_lockprob = lockprob;
    this->subdivobject_01_subdivider_setProb(prob);
    this->subdivobject_01_subdivider_setPattern(pattern);
    divarg = (divarg > 16384 ? 16384 : (divarg < 1 ? 1 : divarg));
    this->subdivobject_01_subdivider_setDiv(divarg);

    if (syncupdate == 0) {
        this->subdivobject_01_subdivider_resolveSync();
    }

    number div = (number)(this->subdivobject_01_subdivider_div);

    if (this->subdivobject_01_subdivider_posstep >= div) {
        this->subdivobject_01_subdivider_posstep = 0;
    }

    bool steppedalready = false;
    number val = (number)(x);

    if ((bool)(this->subdivobject_01_subdivider_detectReset(
        val,
        this->subdivobject_01_subdivider_prev,
        this->subdivobject_01_subdivider_prev2
    ))) {
        this->subdivobject_01_subdivider_posstep = 0;

        if (syncupdate > 0) {
            this->subdivobject_01_subdivider_resolveSync();
            div = this->subdivobject_01_subdivider_div;
        }

        if ((bool)(this->subdivobject_01_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_01_subdivider_lockprob))) {
            if (this->subdivobject_01_subdivider_probstep >= this->subdivobject_01_subdivider_prob->length) {
                this->subdivobject_01_subdivider_probstep = 0;
            }

            steppedalready = true;
        }

        this->subdivobject_01_subdivider_playing = this->subdivobject_01_subdivider_isPlaying(this->subdivobject_01_subdivider_posstep);
    }

    this->subdivobject_01_subdivider_prev2 = this->subdivobject_01_subdivider_prev;
    this->subdivobject_01_subdivider_prev = val;
    val *= div;
    this->subdivobject_01_subdivider_nextposstep = trunc(val);
    val -= this->subdivobject_01_subdivider_patternphase;
    val /= this->subdivobject_01_subdivider_num;
    this->subdivobject_01_subdivider_nextoutstep = trunc(val);

    if (this->subdivobject_01_subdivider_nextoutstep != this->subdivobject_01_subdivider_outstep) {
        if (syncupdate == 1) {
            this->subdivobject_01_subdivider_resolveSync();
            div = this->subdivobject_01_subdivider_div;
        }

        this->subdivobject_01_subdivider_outstep = this->subdivobject_01_subdivider_nextoutstep;

        if ((bool)(this->subdivobject_01_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_01_subdivider_lockprob)) && (bool)(!(bool)(steppedalready))) {
            this->subdivobject_01_subdivider_probstep += 1;

            if (this->subdivobject_01_subdivider_probstep >= this->subdivobject_01_subdivider_prob->length) {
                this->subdivobject_01_subdivider_probstep = 0;
            }
        }
    }

    if (this->subdivobject_01_subdivider_nextposstep != this->subdivobject_01_subdivider_posstep) {
        this->subdivobject_01_subdivider_posstep = this->subdivobject_01_subdivider_nextposstep;
        this->subdivobject_01_subdivider_playing = this->subdivobject_01_subdivider_isPlaying(this->subdivobject_01_subdivider_posstep);
    }

    number out1; number out2;

    if ((bool)(this->subdivobject_01_subdivider_playing)) {
        out1 = val - this->subdivobject_01_subdivider_nextoutstep;
        out2 = this->subdivobject_01_subdivider_nextoutstep;
        this->subdivobject_01_subdivider_lastoutstep = this->subdivobject_01_subdivider_nextoutstep;
    } else {
        out1 = 0.0;
        out2 = (silentmode == 0 ? this->subdivobject_01_subdivider_lastoutstep : -1);
    }

    return {out1, out2};
}

void subdivobject_01_dspsetup(bool force) {
    if ((bool)(this->subdivobject_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->subdivobject_01_setupDone = true;
    this->subdivobject_01_subdivider_dspsetup();
}

void phasor_01_dspsetup(bool force) {
    if ((bool)(this->phasor_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->phasor_01_conv = (number)1 / this->samplerate();
    this->phasor_01_setupDone = true;
}

void subdivobject_02_subdivider_rebuild() {
    number i; number j; number c; number totaltime; number totalsteps;

    if (this->subdivobject_02_subdivider_pattern->length == 0) {
        this->subdivobject_02_subdivider_p2length = 0;
        this->subdivobject_02_subdivider_patternstep = 0;
        this->subdivobject_02_subdivider_patternphase = 0;
        return;
    }

    totaltime = 0;

    for (i = 0; i < this->subdivobject_02_subdivider_pattern->length; i++) {
        totaltime += this->subdivobject_02_subdivider_pattern[(Index)i];
    }

    i = this->safemod(totaltime, this->subdivobject_02_subdivider_div);
    totalsteps = (i == 0 ? totaltime : totaltime + (this->subdivobject_02_subdivider_div - i));

    if (this->subdivobject_02_subdivider_p2length != totalsteps) {
        this->subdivobject_02_subdivider_p2length = totalsteps;
    }

    this->subdivobject_02_subdivider_p2 = {};
    this->subdivobject_02_subdivider_synco = {};
    this->subdivobject_02_subdivider_p2prob = {};

    for (number i = 0; i < totalsteps; i++) {
        this->subdivobject_02_subdivider_p2->push(1);
        this->subdivobject_02_subdivider_synco->push(0);
        this->subdivobject_02_subdivider_p2prob->push(1.0);
    }

    totaltime = 0;
    c = 0;

    for (i = 0; i < this->subdivobject_02_subdivider_pattern->length; i++) {
        number nextvalue = (number)(this->subdivobject_02_subdivider_pattern[(Index)i]);
        number nextstart = (number)(this->safemod(totaltime, this->subdivobject_02_subdivider_div));
        number pv;

        if (this->subdivobject_02_subdivider_prob->length > 0) {
            number ps = (number)(this->safemod(i, this->subdivobject_02_subdivider_prob->length));
            pv = this->subdivobject_02_subdivider_prob[(Index)ps];
        } else {
            pv = 1.0;
        }

        for (j = 0; j < nextvalue; j++) {
            this->subdivobject_02_subdivider_p2[(Index)c] = nextvalue;

            if (nextvalue > 1)
                this->subdivobject_02_subdivider_synco[(Index)c] = nextstart;
            else
                this->subdivobject_02_subdivider_synco[(Index)c] = 0;

            if (j == 0)
                this->subdivobject_02_subdivider_p2prob[(Index)c] = pv;
            else
                this->subdivobject_02_subdivider_p2prob[(Index)c] = -1;

            c++;
        }

        totaltime += nextvalue;
    }

    for (j = 0; c < totalsteps; c++, j++) {
        number pv = 1.0;
        this->subdivobject_02_subdivider_p2[(Index)c] = totalsteps - totaltime;
        this->subdivobject_02_subdivider_synco[(Index)c] = this->safemod(totaltime, this->subdivobject_02_subdivider_div);

        if (j == 0) {
            if (this->subdivobject_02_subdivider_prob->length > 0) {
                number ps = (number)(this->safemod(i, this->subdivobject_02_subdivider_prob->length));
                pv = this->subdivobject_02_subdivider_prob[(Index)ps];
            } else {
                pv = 1.0;
            }
        } else {
            pv = -1.0;
        }

        this->subdivobject_02_subdivider_p2prob[(Index)c] = pv;
    }
}

void subdivobject_02_subdivider_reset() {
    this->subdivobject_02_subdivider_outstep = 0;
    this->subdivobject_02_subdivider_posstep = 0;
    this->subdivobject_02_subdivider_probstep = 0;
    this->subdivobject_02_subdivider_lastoutstep = 0;
    this->subdivobject_02_subdivider_div = 1;
    this->subdivobject_02_subdivider_nextdiv = 1;
    this->subdivobject_02_subdivider_resolveSync();
    this->subdivobject_02_subdivider_playing = this->subdivobject_02_subdivider_isPlaying(0);
    this->subdivobject_02_subdivider_patternstep = 0;
}

void subdivobject_02_subdivider_dspsetup() {
    this->subdivobject_02_subdivider_reset();
    this->subdivobject_02_subdivider_rebuild();
}

void subdivobject_02_subdivider_init() {
    this->subdivobject_02_subdivider_reset();
    this->subdivobject_02_subdivider_rebuild();
}

void subdivobject_02_subdivider_setProb(list prob) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_02_subdivider_prob, prob)[0]))) {
        this->subdivobject_02_subdivider_prob = {};

        if (prob->length > 0) {
            for (number i = 0; i < prob->length; i++) {
                this->subdivobject_02_subdivider_prob->push(
                    (prob[(Index)i] > 1.0 ? 1.0 : (prob[(Index)i] < 0.0 ? 0.0 : prob[(Index)i]))
                );
            }
        }

        this->subdivobject_02_subdivider_rebuild();
    }
}

void subdivobject_02_subdivider_setPattern(list pattern) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_02_subdivider_pattern, pattern)[0]))) {
        this->subdivobject_02_subdivider_pattern = {};

        if (pattern->length > 0) {
            for (number i = 0; i < pattern->length; i++) {
                this->subdivobject_02_subdivider_pattern->push(
                    (pattern[(Index)i] > this->subdivobject_02_subdivider_div ? this->subdivobject_02_subdivider_div : (pattern[(Index)i] < 1 ? 1 : pattern[(Index)i]))
                );
            }
        }

        this->subdivobject_02_subdivider_patternstep = 0;
        this->subdivobject_02_subdivider_rebuild();
    }
}

void subdivobject_02_subdivider_setDiv(int div) {
    if (div != this->subdivobject_02_subdivider_nextdiv) {
        this->subdivobject_02_subdivider_nextdiv = div;
    }
}

number subdivobject_02_subdivider_getPatternNumerator(number step) {
    number value = (number)(this->subdivobject_02_subdivider_p2[(Index)(this->subdivobject_02_subdivider_patternstep + step)]);
    this->subdivobject_02_subdivider_patternphase = this->subdivobject_02_subdivider_synco[(Index)(this->subdivobject_02_subdivider_patternstep + step)];

    if (step == this->subdivobject_02_subdivider_div - 1) {
        if (this->subdivobject_02_subdivider_patternstep + this->subdivobject_02_subdivider_div >= this->subdivobject_02_subdivider_p2length) {
            this->subdivobject_02_subdivider_patternstep = 0;
        } else {
            this->subdivobject_02_subdivider_patternstep += this->subdivobject_02_subdivider_div;
        }
    }

    return value;
}

bool subdivobject_02_subdivider_getPatternProbability(number step, bool playing) {
    number pv = (number)(this->subdivobject_02_subdivider_p2prob[(Index)(this->subdivobject_02_subdivider_patternstep + step)]);

    if (pv == -1) {
        return playing;
    }

    if (pv == 0) {
        return false;
    }

    if (pv == 1) {
        return true;
    }

    return this->random(0, 1) < pv;
}

bool subdivobject_02_subdivider_isPlaying(number step) {
    bool result = false;
    bool decided = false;
    number pv;

    if (this->subdivobject_02_subdivider_prob->length > 0 && (bool)(!(bool)(this->subdivobject_02_subdivider_lockprob))) {
        number ps = (number)(this->subdivobject_02_subdivider_probstep);

        if (ps >= this->subdivobject_02_subdivider_prob->length) {
            ps = 0;
        }

        pv = this->subdivobject_02_subdivider_prob[(Index)ps];
        result = this->random(0, 1) < pv;
        decided = true;
    }

    if (this->subdivobject_02_subdivider_pattern->length > 0) {
        if ((bool)(!(bool)(decided))) {
            result = this->subdivobject_02_subdivider_getPatternProbability(step, this->subdivobject_02_subdivider_playing);
        }

        this->subdivobject_02_subdivider_num = this->subdivobject_02_subdivider_getPatternNumerator(step);
    } else {
        this->subdivobject_02_subdivider_num = 1;

        if ((bool)(this->subdivobject_02_subdivider_prob->length) && (bool)(!(bool)(decided))) {
            number index = (number)(this->safemod(step, this->subdivobject_02_subdivider_prob->length));
            pv = this->subdivobject_02_subdivider_prob[(Index)index];

            if (pv == 0) {
                return false;
            }

            if (pv == 1) {
                return true;
            }

            return this->random(0, 1) < pv;
        } else if ((bool)(!(bool)(decided))) {
            result = true;
        }
    }

    return result;
}

bool subdivobject_02_subdivider_detectReset(number v, number p, number p2) {
    if (p2 < p && v < p && p - v > 0.25) {
        return true;
    }

    if (p2 > p && v > p && v - p > 0.25) {
        return true;
    }

    return false;
}

void subdivobject_02_subdivider_resolveSync() {
    if (this->subdivobject_02_subdivider_nextdiv != this->subdivobject_02_subdivider_div) {
        this->subdivobject_02_subdivider_div = this->subdivobject_02_subdivider_nextdiv;

        if (this->subdivobject_02_subdivider_pattern->length > 0) {
            this->subdivobject_02_subdivider_rebuild();
        }
    }
}

array<number, 2> subdivobject_02_subdivider_next(
    number x,
    number divarg,
    Int syncupdate,
    list prob,
    list pattern,
    bool lockprob,
    bool silentmode
) {
    this->subdivobject_02_subdivider_lockprob = lockprob;
    this->subdivobject_02_subdivider_setProb(prob);
    this->subdivobject_02_subdivider_setPattern(pattern);
    divarg = (divarg > 16384 ? 16384 : (divarg < 1 ? 1 : divarg));
    this->subdivobject_02_subdivider_setDiv(divarg);

    if (syncupdate == 0) {
        this->subdivobject_02_subdivider_resolveSync();
    }

    number div = (number)(this->subdivobject_02_subdivider_div);

    if (this->subdivobject_02_subdivider_posstep >= div) {
        this->subdivobject_02_subdivider_posstep = 0;
    }

    bool steppedalready = false;
    number val = (number)(x);

    if ((bool)(this->subdivobject_02_subdivider_detectReset(
        val,
        this->subdivobject_02_subdivider_prev,
        this->subdivobject_02_subdivider_prev2
    ))) {
        this->subdivobject_02_subdivider_posstep = 0;

        if (syncupdate > 0) {
            this->subdivobject_02_subdivider_resolveSync();
            div = this->subdivobject_02_subdivider_div;
        }

        if ((bool)(this->subdivobject_02_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_02_subdivider_lockprob))) {
            if (this->subdivobject_02_subdivider_probstep >= this->subdivobject_02_subdivider_prob->length) {
                this->subdivobject_02_subdivider_probstep = 0;
            }

            steppedalready = true;
        }

        this->subdivobject_02_subdivider_playing = this->subdivobject_02_subdivider_isPlaying(this->subdivobject_02_subdivider_posstep);
    }

    this->subdivobject_02_subdivider_prev2 = this->subdivobject_02_subdivider_prev;
    this->subdivobject_02_subdivider_prev = val;
    val *= div;
    this->subdivobject_02_subdivider_nextposstep = trunc(val);
    val -= this->subdivobject_02_subdivider_patternphase;
    val /= this->subdivobject_02_subdivider_num;
    this->subdivobject_02_subdivider_nextoutstep = trunc(val);

    if (this->subdivobject_02_subdivider_nextoutstep != this->subdivobject_02_subdivider_outstep) {
        if (syncupdate == 1) {
            this->subdivobject_02_subdivider_resolveSync();
            div = this->subdivobject_02_subdivider_div;
        }

        this->subdivobject_02_subdivider_outstep = this->subdivobject_02_subdivider_nextoutstep;

        if ((bool)(this->subdivobject_02_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_02_subdivider_lockprob)) && (bool)(!(bool)(steppedalready))) {
            this->subdivobject_02_subdivider_probstep += 1;

            if (this->subdivobject_02_subdivider_probstep >= this->subdivobject_02_subdivider_prob->length) {
                this->subdivobject_02_subdivider_probstep = 0;
            }
        }
    }

    if (this->subdivobject_02_subdivider_nextposstep != this->subdivobject_02_subdivider_posstep) {
        this->subdivobject_02_subdivider_posstep = this->subdivobject_02_subdivider_nextposstep;
        this->subdivobject_02_subdivider_playing = this->subdivobject_02_subdivider_isPlaying(this->subdivobject_02_subdivider_posstep);
    }

    number out1; number out2;

    if ((bool)(this->subdivobject_02_subdivider_playing)) {
        out1 = val - this->subdivobject_02_subdivider_nextoutstep;
        out2 = this->subdivobject_02_subdivider_nextoutstep;
        this->subdivobject_02_subdivider_lastoutstep = this->subdivobject_02_subdivider_nextoutstep;
    } else {
        out1 = 0.0;
        out2 = (silentmode == 0 ? this->subdivobject_02_subdivider_lastoutstep : -1);
    }

    return {out1, out2};
}

void subdivobject_02_dspsetup(bool force) {
    if ((bool)(this->subdivobject_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->subdivobject_02_setupDone = true;
    this->subdivobject_02_subdivider_dspsetup();
}

void phasor_02_dspsetup(bool force) {
    if ((bool)(this->phasor_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->phasor_02_conv = (number)1 / this->samplerate();
    this->phasor_02_setupDone = true;
}

void subdivobject_03_subdivider_rebuild() {
    number i; number j; number c; number totaltime; number totalsteps;

    if (this->subdivobject_03_subdivider_pattern->length == 0) {
        this->subdivobject_03_subdivider_p2length = 0;
        this->subdivobject_03_subdivider_patternstep = 0;
        this->subdivobject_03_subdivider_patternphase = 0;
        return;
    }

    totaltime = 0;

    for (i = 0; i < this->subdivobject_03_subdivider_pattern->length; i++) {
        totaltime += this->subdivobject_03_subdivider_pattern[(Index)i];
    }

    i = this->safemod(totaltime, this->subdivobject_03_subdivider_div);
    totalsteps = (i == 0 ? totaltime : totaltime + (this->subdivobject_03_subdivider_div - i));

    if (this->subdivobject_03_subdivider_p2length != totalsteps) {
        this->subdivobject_03_subdivider_p2length = totalsteps;
    }

    this->subdivobject_03_subdivider_p2 = {};
    this->subdivobject_03_subdivider_synco = {};
    this->subdivobject_03_subdivider_p2prob = {};

    for (number i = 0; i < totalsteps; i++) {
        this->subdivobject_03_subdivider_p2->push(1);
        this->subdivobject_03_subdivider_synco->push(0);
        this->subdivobject_03_subdivider_p2prob->push(1.0);
    }

    totaltime = 0;
    c = 0;

    for (i = 0; i < this->subdivobject_03_subdivider_pattern->length; i++) {
        number nextvalue = (number)(this->subdivobject_03_subdivider_pattern[(Index)i]);
        number nextstart = (number)(this->safemod(totaltime, this->subdivobject_03_subdivider_div));
        number pv;

        if (this->subdivobject_03_subdivider_prob->length > 0) {
            number ps = (number)(this->safemod(i, this->subdivobject_03_subdivider_prob->length));
            pv = this->subdivobject_03_subdivider_prob[(Index)ps];
        } else {
            pv = 1.0;
        }

        for (j = 0; j < nextvalue; j++) {
            this->subdivobject_03_subdivider_p2[(Index)c] = nextvalue;

            if (nextvalue > 1)
                this->subdivobject_03_subdivider_synco[(Index)c] = nextstart;
            else
                this->subdivobject_03_subdivider_synco[(Index)c] = 0;

            if (j == 0)
                this->subdivobject_03_subdivider_p2prob[(Index)c] = pv;
            else
                this->subdivobject_03_subdivider_p2prob[(Index)c] = -1;

            c++;
        }

        totaltime += nextvalue;
    }

    for (j = 0; c < totalsteps; c++, j++) {
        number pv = 1.0;
        this->subdivobject_03_subdivider_p2[(Index)c] = totalsteps - totaltime;
        this->subdivobject_03_subdivider_synco[(Index)c] = this->safemod(totaltime, this->subdivobject_03_subdivider_div);

        if (j == 0) {
            if (this->subdivobject_03_subdivider_prob->length > 0) {
                number ps = (number)(this->safemod(i, this->subdivobject_03_subdivider_prob->length));
                pv = this->subdivobject_03_subdivider_prob[(Index)ps];
            } else {
                pv = 1.0;
            }
        } else {
            pv = -1.0;
        }

        this->subdivobject_03_subdivider_p2prob[(Index)c] = pv;
    }
}

void subdivobject_03_subdivider_reset() {
    this->subdivobject_03_subdivider_outstep = 0;
    this->subdivobject_03_subdivider_posstep = 0;
    this->subdivobject_03_subdivider_probstep = 0;
    this->subdivobject_03_subdivider_lastoutstep = 0;
    this->subdivobject_03_subdivider_div = 1;
    this->subdivobject_03_subdivider_nextdiv = 1;
    this->subdivobject_03_subdivider_resolveSync();
    this->subdivobject_03_subdivider_playing = this->subdivobject_03_subdivider_isPlaying(0);
    this->subdivobject_03_subdivider_patternstep = 0;
}

void subdivobject_03_subdivider_dspsetup() {
    this->subdivobject_03_subdivider_reset();
    this->subdivobject_03_subdivider_rebuild();
}

void subdivobject_03_subdivider_init() {
    this->subdivobject_03_subdivider_reset();
    this->subdivobject_03_subdivider_rebuild();
}

void subdivobject_03_subdivider_setProb(list prob) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_03_subdivider_prob, prob)[0]))) {
        this->subdivobject_03_subdivider_prob = {};

        if (prob->length > 0) {
            for (number i = 0; i < prob->length; i++) {
                this->subdivobject_03_subdivider_prob->push(
                    (prob[(Index)i] > 1.0 ? 1.0 : (prob[(Index)i] < 0.0 ? 0.0 : prob[(Index)i]))
                );
            }
        }

        this->subdivobject_03_subdivider_rebuild();
    }
}

void subdivobject_03_subdivider_setPattern(list pattern) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_03_subdivider_pattern, pattern)[0]))) {
        this->subdivobject_03_subdivider_pattern = {};

        if (pattern->length > 0) {
            for (number i = 0; i < pattern->length; i++) {
                this->subdivobject_03_subdivider_pattern->push(
                    (pattern[(Index)i] > this->subdivobject_03_subdivider_div ? this->subdivobject_03_subdivider_div : (pattern[(Index)i] < 1 ? 1 : pattern[(Index)i]))
                );
            }
        }

        this->subdivobject_03_subdivider_patternstep = 0;
        this->subdivobject_03_subdivider_rebuild();
    }
}

void subdivobject_03_subdivider_setDiv(int div) {
    if (div != this->subdivobject_03_subdivider_nextdiv) {
        this->subdivobject_03_subdivider_nextdiv = div;
    }
}

number subdivobject_03_subdivider_getPatternNumerator(number step) {
    number value = (number)(this->subdivobject_03_subdivider_p2[(Index)(this->subdivobject_03_subdivider_patternstep + step)]);
    this->subdivobject_03_subdivider_patternphase = this->subdivobject_03_subdivider_synco[(Index)(this->subdivobject_03_subdivider_patternstep + step)];

    if (step == this->subdivobject_03_subdivider_div - 1) {
        if (this->subdivobject_03_subdivider_patternstep + this->subdivobject_03_subdivider_div >= this->subdivobject_03_subdivider_p2length) {
            this->subdivobject_03_subdivider_patternstep = 0;
        } else {
            this->subdivobject_03_subdivider_patternstep += this->subdivobject_03_subdivider_div;
        }
    }

    return value;
}

bool subdivobject_03_subdivider_getPatternProbability(number step, bool playing) {
    number pv = (number)(this->subdivobject_03_subdivider_p2prob[(Index)(this->subdivobject_03_subdivider_patternstep + step)]);

    if (pv == -1) {
        return playing;
    }

    if (pv == 0) {
        return false;
    }

    if (pv == 1) {
        return true;
    }

    return this->random(0, 1) < pv;
}

bool subdivobject_03_subdivider_isPlaying(number step) {
    bool result = false;
    bool decided = false;
    number pv;

    if (this->subdivobject_03_subdivider_prob->length > 0 && (bool)(!(bool)(this->subdivobject_03_subdivider_lockprob))) {
        number ps = (number)(this->subdivobject_03_subdivider_probstep);

        if (ps >= this->subdivobject_03_subdivider_prob->length) {
            ps = 0;
        }

        pv = this->subdivobject_03_subdivider_prob[(Index)ps];
        result = this->random(0, 1) < pv;
        decided = true;
    }

    if (this->subdivobject_03_subdivider_pattern->length > 0) {
        if ((bool)(!(bool)(decided))) {
            result = this->subdivobject_03_subdivider_getPatternProbability(step, this->subdivobject_03_subdivider_playing);
        }

        this->subdivobject_03_subdivider_num = this->subdivobject_03_subdivider_getPatternNumerator(step);
    } else {
        this->subdivobject_03_subdivider_num = 1;

        if ((bool)(this->subdivobject_03_subdivider_prob->length) && (bool)(!(bool)(decided))) {
            number index = (number)(this->safemod(step, this->subdivobject_03_subdivider_prob->length));
            pv = this->subdivobject_03_subdivider_prob[(Index)index];

            if (pv == 0) {
                return false;
            }

            if (pv == 1) {
                return true;
            }

            return this->random(0, 1) < pv;
        } else if ((bool)(!(bool)(decided))) {
            result = true;
        }
    }

    return result;
}

bool subdivobject_03_subdivider_detectReset(number v, number p, number p2) {
    if (p2 < p && v < p && p - v > 0.25) {
        return true;
    }

    if (p2 > p && v > p && v - p > 0.25) {
        return true;
    }

    return false;
}

void subdivobject_03_subdivider_resolveSync() {
    if (this->subdivobject_03_subdivider_nextdiv != this->subdivobject_03_subdivider_div) {
        this->subdivobject_03_subdivider_div = this->subdivobject_03_subdivider_nextdiv;

        if (this->subdivobject_03_subdivider_pattern->length > 0) {
            this->subdivobject_03_subdivider_rebuild();
        }
    }
}

array<number, 2> subdivobject_03_subdivider_next(
    number x,
    number divarg,
    Int syncupdate,
    list prob,
    list pattern,
    bool lockprob,
    bool silentmode
) {
    this->subdivobject_03_subdivider_lockprob = lockprob;
    this->subdivobject_03_subdivider_setProb(prob);
    this->subdivobject_03_subdivider_setPattern(pattern);
    divarg = (divarg > 16384 ? 16384 : (divarg < 1 ? 1 : divarg));
    this->subdivobject_03_subdivider_setDiv(divarg);

    if (syncupdate == 0) {
        this->subdivobject_03_subdivider_resolveSync();
    }

    number div = (number)(this->subdivobject_03_subdivider_div);

    if (this->subdivobject_03_subdivider_posstep >= div) {
        this->subdivobject_03_subdivider_posstep = 0;
    }

    bool steppedalready = false;
    number val = (number)(x);

    if ((bool)(this->subdivobject_03_subdivider_detectReset(
        val,
        this->subdivobject_03_subdivider_prev,
        this->subdivobject_03_subdivider_prev2
    ))) {
        this->subdivobject_03_subdivider_posstep = 0;

        if (syncupdate > 0) {
            this->subdivobject_03_subdivider_resolveSync();
            div = this->subdivobject_03_subdivider_div;
        }

        if ((bool)(this->subdivobject_03_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_03_subdivider_lockprob))) {
            if (this->subdivobject_03_subdivider_probstep >= this->subdivobject_03_subdivider_prob->length) {
                this->subdivobject_03_subdivider_probstep = 0;
            }

            steppedalready = true;
        }

        this->subdivobject_03_subdivider_playing = this->subdivobject_03_subdivider_isPlaying(this->subdivobject_03_subdivider_posstep);
    }

    this->subdivobject_03_subdivider_prev2 = this->subdivobject_03_subdivider_prev;
    this->subdivobject_03_subdivider_prev = val;
    val *= div;
    this->subdivobject_03_subdivider_nextposstep = trunc(val);
    val -= this->subdivobject_03_subdivider_patternphase;
    val /= this->subdivobject_03_subdivider_num;
    this->subdivobject_03_subdivider_nextoutstep = trunc(val);

    if (this->subdivobject_03_subdivider_nextoutstep != this->subdivobject_03_subdivider_outstep) {
        if (syncupdate == 1) {
            this->subdivobject_03_subdivider_resolveSync();
            div = this->subdivobject_03_subdivider_div;
        }

        this->subdivobject_03_subdivider_outstep = this->subdivobject_03_subdivider_nextoutstep;

        if ((bool)(this->subdivobject_03_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_03_subdivider_lockprob)) && (bool)(!(bool)(steppedalready))) {
            this->subdivobject_03_subdivider_probstep += 1;

            if (this->subdivobject_03_subdivider_probstep >= this->subdivobject_03_subdivider_prob->length) {
                this->subdivobject_03_subdivider_probstep = 0;
            }
        }
    }

    if (this->subdivobject_03_subdivider_nextposstep != this->subdivobject_03_subdivider_posstep) {
        this->subdivobject_03_subdivider_posstep = this->subdivobject_03_subdivider_nextposstep;
        this->subdivobject_03_subdivider_playing = this->subdivobject_03_subdivider_isPlaying(this->subdivobject_03_subdivider_posstep);
    }

    number out1; number out2;

    if ((bool)(this->subdivobject_03_subdivider_playing)) {
        out1 = val - this->subdivobject_03_subdivider_nextoutstep;
        out2 = this->subdivobject_03_subdivider_nextoutstep;
        this->subdivobject_03_subdivider_lastoutstep = this->subdivobject_03_subdivider_nextoutstep;
    } else {
        out1 = 0.0;
        out2 = (silentmode == 0 ? this->subdivobject_03_subdivider_lastoutstep : -1);
    }

    return {out1, out2};
}

void subdivobject_03_dspsetup(bool force) {
    if ((bool)(this->subdivobject_03_setupDone) && (bool)(!(bool)(force)))
        return;

    this->subdivobject_03_setupDone = true;
    this->subdivobject_03_subdivider_dspsetup();
}

void phasor_03_dspsetup(bool force) {
    if ((bool)(this->phasor_03_setupDone) && (bool)(!(bool)(force)))
        return;

    this->phasor_03_conv = (number)1 / this->samplerate();
    this->phasor_03_setupDone = true;
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

void subdivobject_04_subdivider_rebuild() {
    number i; number j; number c; number totaltime; number totalsteps;

    if (this->subdivobject_04_subdivider_pattern->length == 0) {
        this->subdivobject_04_subdivider_p2length = 0;
        this->subdivobject_04_subdivider_patternstep = 0;
        this->subdivobject_04_subdivider_patternphase = 0;
        return;
    }

    totaltime = 0;

    for (i = 0; i < this->subdivobject_04_subdivider_pattern->length; i++) {
        totaltime += this->subdivobject_04_subdivider_pattern[(Index)i];
    }

    i = this->safemod(totaltime, this->subdivobject_04_subdivider_div);
    totalsteps = (i == 0 ? totaltime : totaltime + (this->subdivobject_04_subdivider_div - i));

    if (this->subdivobject_04_subdivider_p2length != totalsteps) {
        this->subdivobject_04_subdivider_p2length = totalsteps;
    }

    this->subdivobject_04_subdivider_p2 = {};
    this->subdivobject_04_subdivider_synco = {};
    this->subdivobject_04_subdivider_p2prob = {};

    for (number i = 0; i < totalsteps; i++) {
        this->subdivobject_04_subdivider_p2->push(1);
        this->subdivobject_04_subdivider_synco->push(0);
        this->subdivobject_04_subdivider_p2prob->push(1.0);
    }

    totaltime = 0;
    c = 0;

    for (i = 0; i < this->subdivobject_04_subdivider_pattern->length; i++) {
        number nextvalue = (number)(this->subdivobject_04_subdivider_pattern[(Index)i]);
        number nextstart = (number)(this->safemod(totaltime, this->subdivobject_04_subdivider_div));
        number pv;

        if (this->subdivobject_04_subdivider_prob->length > 0) {
            number ps = (number)(this->safemod(i, this->subdivobject_04_subdivider_prob->length));
            pv = this->subdivobject_04_subdivider_prob[(Index)ps];
        } else {
            pv = 1.0;
        }

        for (j = 0; j < nextvalue; j++) {
            this->subdivobject_04_subdivider_p2[(Index)c] = nextvalue;

            if (nextvalue > 1)
                this->subdivobject_04_subdivider_synco[(Index)c] = nextstart;
            else
                this->subdivobject_04_subdivider_synco[(Index)c] = 0;

            if (j == 0)
                this->subdivobject_04_subdivider_p2prob[(Index)c] = pv;
            else
                this->subdivobject_04_subdivider_p2prob[(Index)c] = -1;

            c++;
        }

        totaltime += nextvalue;
    }

    for (j = 0; c < totalsteps; c++, j++) {
        number pv = 1.0;
        this->subdivobject_04_subdivider_p2[(Index)c] = totalsteps - totaltime;
        this->subdivobject_04_subdivider_synco[(Index)c] = this->safemod(totaltime, this->subdivobject_04_subdivider_div);

        if (j == 0) {
            if (this->subdivobject_04_subdivider_prob->length > 0) {
                number ps = (number)(this->safemod(i, this->subdivobject_04_subdivider_prob->length));
                pv = this->subdivobject_04_subdivider_prob[(Index)ps];
            } else {
                pv = 1.0;
            }
        } else {
            pv = -1.0;
        }

        this->subdivobject_04_subdivider_p2prob[(Index)c] = pv;
    }
}

void subdivobject_04_subdivider_reset() {
    this->subdivobject_04_subdivider_outstep = 0;
    this->subdivobject_04_subdivider_posstep = 0;
    this->subdivobject_04_subdivider_probstep = 0;
    this->subdivobject_04_subdivider_lastoutstep = 0;
    this->subdivobject_04_subdivider_div = 1;
    this->subdivobject_04_subdivider_nextdiv = 1;
    this->subdivobject_04_subdivider_resolveSync();
    this->subdivobject_04_subdivider_playing = this->subdivobject_04_subdivider_isPlaying(0);
    this->subdivobject_04_subdivider_patternstep = 0;
}

void subdivobject_04_subdivider_dspsetup() {
    this->subdivobject_04_subdivider_reset();
    this->subdivobject_04_subdivider_rebuild();
}

void subdivobject_04_subdivider_init() {
    this->subdivobject_04_subdivider_reset();
    this->subdivobject_04_subdivider_rebuild();
}

void subdivobject_04_subdivider_setProb(list prob) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_04_subdivider_prob, prob)[0]))) {
        this->subdivobject_04_subdivider_prob = {};

        if (prob->length > 0) {
            for (number i = 0; i < prob->length; i++) {
                this->subdivobject_04_subdivider_prob->push(
                    (prob[(Index)i] > 1.0 ? 1.0 : (prob[(Index)i] < 0.0 ? 0.0 : prob[(Index)i]))
                );
            }
        }

        this->subdivobject_04_subdivider_rebuild();
    }
}

void subdivobject_04_subdivider_setPattern(list pattern) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_04_subdivider_pattern, pattern)[0]))) {
        this->subdivobject_04_subdivider_pattern = {};

        if (pattern->length > 0) {
            for (number i = 0; i < pattern->length; i++) {
                this->subdivobject_04_subdivider_pattern->push(
                    (pattern[(Index)i] > this->subdivobject_04_subdivider_div ? this->subdivobject_04_subdivider_div : (pattern[(Index)i] < 1 ? 1 : pattern[(Index)i]))
                );
            }
        }

        this->subdivobject_04_subdivider_patternstep = 0;
        this->subdivobject_04_subdivider_rebuild();
    }
}

void subdivobject_04_subdivider_setDiv(int div) {
    if (div != this->subdivobject_04_subdivider_nextdiv) {
        this->subdivobject_04_subdivider_nextdiv = div;
    }
}

number subdivobject_04_subdivider_getPatternNumerator(number step) {
    number value = (number)(this->subdivobject_04_subdivider_p2[(Index)(this->subdivobject_04_subdivider_patternstep + step)]);
    this->subdivobject_04_subdivider_patternphase = this->subdivobject_04_subdivider_synco[(Index)(this->subdivobject_04_subdivider_patternstep + step)];

    if (step == this->subdivobject_04_subdivider_div - 1) {
        if (this->subdivobject_04_subdivider_patternstep + this->subdivobject_04_subdivider_div >= this->subdivobject_04_subdivider_p2length) {
            this->subdivobject_04_subdivider_patternstep = 0;
        } else {
            this->subdivobject_04_subdivider_patternstep += this->subdivobject_04_subdivider_div;
        }
    }

    return value;
}

bool subdivobject_04_subdivider_getPatternProbability(number step, bool playing) {
    number pv = (number)(this->subdivobject_04_subdivider_p2prob[(Index)(this->subdivobject_04_subdivider_patternstep + step)]);

    if (pv == -1) {
        return playing;
    }

    if (pv == 0) {
        return false;
    }

    if (pv == 1) {
        return true;
    }

    return this->random(0, 1) < pv;
}

bool subdivobject_04_subdivider_isPlaying(number step) {
    bool result = false;
    bool decided = false;
    number pv;

    if (this->subdivobject_04_subdivider_prob->length > 0 && (bool)(!(bool)(this->subdivobject_04_subdivider_lockprob))) {
        number ps = (number)(this->subdivobject_04_subdivider_probstep);

        if (ps >= this->subdivobject_04_subdivider_prob->length) {
            ps = 0;
        }

        pv = this->subdivobject_04_subdivider_prob[(Index)ps];
        result = this->random(0, 1) < pv;
        decided = true;
    }

    if (this->subdivobject_04_subdivider_pattern->length > 0) {
        if ((bool)(!(bool)(decided))) {
            result = this->subdivobject_04_subdivider_getPatternProbability(step, this->subdivobject_04_subdivider_playing);
        }

        this->subdivobject_04_subdivider_num = this->subdivobject_04_subdivider_getPatternNumerator(step);
    } else {
        this->subdivobject_04_subdivider_num = 1;

        if ((bool)(this->subdivobject_04_subdivider_prob->length) && (bool)(!(bool)(decided))) {
            number index = (number)(this->safemod(step, this->subdivobject_04_subdivider_prob->length));
            pv = this->subdivobject_04_subdivider_prob[(Index)index];

            if (pv == 0) {
                return false;
            }

            if (pv == 1) {
                return true;
            }

            return this->random(0, 1) < pv;
        } else if ((bool)(!(bool)(decided))) {
            result = true;
        }
    }

    return result;
}

bool subdivobject_04_subdivider_detectReset(number v, number p, number p2) {
    if (p2 < p && v < p && p - v > 0.25) {
        return true;
    }

    if (p2 > p && v > p && v - p > 0.25) {
        return true;
    }

    return false;
}

void subdivobject_04_subdivider_resolveSync() {
    if (this->subdivobject_04_subdivider_nextdiv != this->subdivobject_04_subdivider_div) {
        this->subdivobject_04_subdivider_div = this->subdivobject_04_subdivider_nextdiv;

        if (this->subdivobject_04_subdivider_pattern->length > 0) {
            this->subdivobject_04_subdivider_rebuild();
        }
    }
}

array<number, 2> subdivobject_04_subdivider_next(
    number x,
    number divarg,
    Int syncupdate,
    list prob,
    list pattern,
    bool lockprob,
    bool silentmode
) {
    this->subdivobject_04_subdivider_lockprob = lockprob;
    this->subdivobject_04_subdivider_setProb(prob);
    this->subdivobject_04_subdivider_setPattern(pattern);
    divarg = (divarg > 16384 ? 16384 : (divarg < 1 ? 1 : divarg));
    this->subdivobject_04_subdivider_setDiv(divarg);

    if (syncupdate == 0) {
        this->subdivobject_04_subdivider_resolveSync();
    }

    number div = (number)(this->subdivobject_04_subdivider_div);

    if (this->subdivobject_04_subdivider_posstep >= div) {
        this->subdivobject_04_subdivider_posstep = 0;
    }

    bool steppedalready = false;
    number val = (number)(x);

    if ((bool)(this->subdivobject_04_subdivider_detectReset(
        val,
        this->subdivobject_04_subdivider_prev,
        this->subdivobject_04_subdivider_prev2
    ))) {
        this->subdivobject_04_subdivider_posstep = 0;

        if (syncupdate > 0) {
            this->subdivobject_04_subdivider_resolveSync();
            div = this->subdivobject_04_subdivider_div;
        }

        if ((bool)(this->subdivobject_04_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_04_subdivider_lockprob))) {
            if (this->subdivobject_04_subdivider_probstep >= this->subdivobject_04_subdivider_prob->length) {
                this->subdivobject_04_subdivider_probstep = 0;
            }

            steppedalready = true;
        }

        this->subdivobject_04_subdivider_playing = this->subdivobject_04_subdivider_isPlaying(this->subdivobject_04_subdivider_posstep);
    }

    this->subdivobject_04_subdivider_prev2 = this->subdivobject_04_subdivider_prev;
    this->subdivobject_04_subdivider_prev = val;
    val *= div;
    this->subdivobject_04_subdivider_nextposstep = trunc(val);
    val -= this->subdivobject_04_subdivider_patternphase;
    val /= this->subdivobject_04_subdivider_num;
    this->subdivobject_04_subdivider_nextoutstep = trunc(val);

    if (this->subdivobject_04_subdivider_nextoutstep != this->subdivobject_04_subdivider_outstep) {
        if (syncupdate == 1) {
            this->subdivobject_04_subdivider_resolveSync();
            div = this->subdivobject_04_subdivider_div;
        }

        this->subdivobject_04_subdivider_outstep = this->subdivobject_04_subdivider_nextoutstep;

        if ((bool)(this->subdivobject_04_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_04_subdivider_lockprob)) && (bool)(!(bool)(steppedalready))) {
            this->subdivobject_04_subdivider_probstep += 1;

            if (this->subdivobject_04_subdivider_probstep >= this->subdivobject_04_subdivider_prob->length) {
                this->subdivobject_04_subdivider_probstep = 0;
            }
        }
    }

    if (this->subdivobject_04_subdivider_nextposstep != this->subdivobject_04_subdivider_posstep) {
        this->subdivobject_04_subdivider_posstep = this->subdivobject_04_subdivider_nextposstep;
        this->subdivobject_04_subdivider_playing = this->subdivobject_04_subdivider_isPlaying(this->subdivobject_04_subdivider_posstep);
    }

    number out1; number out2;

    if ((bool)(this->subdivobject_04_subdivider_playing)) {
        out1 = val - this->subdivobject_04_subdivider_nextoutstep;
        out2 = this->subdivobject_04_subdivider_nextoutstep;
        this->subdivobject_04_subdivider_lastoutstep = this->subdivobject_04_subdivider_nextoutstep;
    } else {
        out1 = 0.0;
        out2 = (silentmode == 0 ? this->subdivobject_04_subdivider_lastoutstep : -1);
    }

    return {out1, out2};
}

void subdivobject_04_dspsetup(bool force) {
    if ((bool)(this->subdivobject_04_setupDone) && (bool)(!(bool)(force)))
        return;

    this->subdivobject_04_setupDone = true;
    this->subdivobject_04_subdivider_dspsetup();
}

void phasor_04_dspsetup(bool force) {
    if ((bool)(this->phasor_04_setupDone) && (bool)(!(bool)(force)))
        return;

    this->phasor_04_conv = (number)1 / this->samplerate();
    this->phasor_04_setupDone = true;
}

void subdivobject_05_subdivider_rebuild() {
    number i; number j; number c; number totaltime; number totalsteps;

    if (this->subdivobject_05_subdivider_pattern->length == 0) {
        this->subdivobject_05_subdivider_p2length = 0;
        this->subdivobject_05_subdivider_patternstep = 0;
        this->subdivobject_05_subdivider_patternphase = 0;
        return;
    }

    totaltime = 0;

    for (i = 0; i < this->subdivobject_05_subdivider_pattern->length; i++) {
        totaltime += this->subdivobject_05_subdivider_pattern[(Index)i];
    }

    i = this->safemod(totaltime, this->subdivobject_05_subdivider_div);
    totalsteps = (i == 0 ? totaltime : totaltime + (this->subdivobject_05_subdivider_div - i));

    if (this->subdivobject_05_subdivider_p2length != totalsteps) {
        this->subdivobject_05_subdivider_p2length = totalsteps;
    }

    this->subdivobject_05_subdivider_p2 = {};
    this->subdivobject_05_subdivider_synco = {};
    this->subdivobject_05_subdivider_p2prob = {};

    for (number i = 0; i < totalsteps; i++) {
        this->subdivobject_05_subdivider_p2->push(1);
        this->subdivobject_05_subdivider_synco->push(0);
        this->subdivobject_05_subdivider_p2prob->push(1.0);
    }

    totaltime = 0;
    c = 0;

    for (i = 0; i < this->subdivobject_05_subdivider_pattern->length; i++) {
        number nextvalue = (number)(this->subdivobject_05_subdivider_pattern[(Index)i]);
        number nextstart = (number)(this->safemod(totaltime, this->subdivobject_05_subdivider_div));
        number pv;

        if (this->subdivobject_05_subdivider_prob->length > 0) {
            number ps = (number)(this->safemod(i, this->subdivobject_05_subdivider_prob->length));
            pv = this->subdivobject_05_subdivider_prob[(Index)ps];
        } else {
            pv = 1.0;
        }

        for (j = 0; j < nextvalue; j++) {
            this->subdivobject_05_subdivider_p2[(Index)c] = nextvalue;

            if (nextvalue > 1)
                this->subdivobject_05_subdivider_synco[(Index)c] = nextstart;
            else
                this->subdivobject_05_subdivider_synco[(Index)c] = 0;

            if (j == 0)
                this->subdivobject_05_subdivider_p2prob[(Index)c] = pv;
            else
                this->subdivobject_05_subdivider_p2prob[(Index)c] = -1;

            c++;
        }

        totaltime += nextvalue;
    }

    for (j = 0; c < totalsteps; c++, j++) {
        number pv = 1.0;
        this->subdivobject_05_subdivider_p2[(Index)c] = totalsteps - totaltime;
        this->subdivobject_05_subdivider_synco[(Index)c] = this->safemod(totaltime, this->subdivobject_05_subdivider_div);

        if (j == 0) {
            if (this->subdivobject_05_subdivider_prob->length > 0) {
                number ps = (number)(this->safemod(i, this->subdivobject_05_subdivider_prob->length));
                pv = this->subdivobject_05_subdivider_prob[(Index)ps];
            } else {
                pv = 1.0;
            }
        } else {
            pv = -1.0;
        }

        this->subdivobject_05_subdivider_p2prob[(Index)c] = pv;
    }
}

void subdivobject_05_subdivider_reset() {
    this->subdivobject_05_subdivider_outstep = 0;
    this->subdivobject_05_subdivider_posstep = 0;
    this->subdivobject_05_subdivider_probstep = 0;
    this->subdivobject_05_subdivider_lastoutstep = 0;
    this->subdivobject_05_subdivider_div = 1;
    this->subdivobject_05_subdivider_nextdiv = 1;
    this->subdivobject_05_subdivider_resolveSync();
    this->subdivobject_05_subdivider_playing = this->subdivobject_05_subdivider_isPlaying(0);
    this->subdivobject_05_subdivider_patternstep = 0;
}

void subdivobject_05_subdivider_dspsetup() {
    this->subdivobject_05_subdivider_reset();
    this->subdivobject_05_subdivider_rebuild();
}

void subdivobject_05_subdivider_init() {
    this->subdivobject_05_subdivider_reset();
    this->subdivobject_05_subdivider_rebuild();
}

void subdivobject_05_subdivider_setProb(list prob) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_05_subdivider_prob, prob)[0]))) {
        this->subdivobject_05_subdivider_prob = {};

        if (prob->length > 0) {
            for (number i = 0; i < prob->length; i++) {
                this->subdivobject_05_subdivider_prob->push(
                    (prob[(Index)i] > 1.0 ? 1.0 : (prob[(Index)i] < 0.0 ? 0.0 : prob[(Index)i]))
                );
            }
        }

        this->subdivobject_05_subdivider_rebuild();
    }
}

void subdivobject_05_subdivider_setPattern(list pattern) {
    if ((bool)(!(bool)(this->listcompare(this->subdivobject_05_subdivider_pattern, pattern)[0]))) {
        this->subdivobject_05_subdivider_pattern = {};

        if (pattern->length > 0) {
            for (number i = 0; i < pattern->length; i++) {
                this->subdivobject_05_subdivider_pattern->push(
                    (pattern[(Index)i] > this->subdivobject_05_subdivider_div ? this->subdivobject_05_subdivider_div : (pattern[(Index)i] < 1 ? 1 : pattern[(Index)i]))
                );
            }
        }

        this->subdivobject_05_subdivider_patternstep = 0;
        this->subdivobject_05_subdivider_rebuild();
    }
}

void subdivobject_05_subdivider_setDiv(int div) {
    if (div != this->subdivobject_05_subdivider_nextdiv) {
        this->subdivobject_05_subdivider_nextdiv = div;
    }
}

number subdivobject_05_subdivider_getPatternNumerator(number step) {
    number value = (number)(this->subdivobject_05_subdivider_p2[(Index)(this->subdivobject_05_subdivider_patternstep + step)]);
    this->subdivobject_05_subdivider_patternphase = this->subdivobject_05_subdivider_synco[(Index)(this->subdivobject_05_subdivider_patternstep + step)];

    if (step == this->subdivobject_05_subdivider_div - 1) {
        if (this->subdivobject_05_subdivider_patternstep + this->subdivobject_05_subdivider_div >= this->subdivobject_05_subdivider_p2length) {
            this->subdivobject_05_subdivider_patternstep = 0;
        } else {
            this->subdivobject_05_subdivider_patternstep += this->subdivobject_05_subdivider_div;
        }
    }

    return value;
}

bool subdivobject_05_subdivider_getPatternProbability(number step, bool playing) {
    number pv = (number)(this->subdivobject_05_subdivider_p2prob[(Index)(this->subdivobject_05_subdivider_patternstep + step)]);

    if (pv == -1) {
        return playing;
    }

    if (pv == 0) {
        return false;
    }

    if (pv == 1) {
        return true;
    }

    return this->random(0, 1) < pv;
}

bool subdivobject_05_subdivider_isPlaying(number step) {
    bool result = false;
    bool decided = false;
    number pv;

    if (this->subdivobject_05_subdivider_prob->length > 0 && (bool)(!(bool)(this->subdivobject_05_subdivider_lockprob))) {
        number ps = (number)(this->subdivobject_05_subdivider_probstep);

        if (ps >= this->subdivobject_05_subdivider_prob->length) {
            ps = 0;
        }

        pv = this->subdivobject_05_subdivider_prob[(Index)ps];
        result = this->random(0, 1) < pv;
        decided = true;
    }

    if (this->subdivobject_05_subdivider_pattern->length > 0) {
        if ((bool)(!(bool)(decided))) {
            result = this->subdivobject_05_subdivider_getPatternProbability(step, this->subdivobject_05_subdivider_playing);
        }

        this->subdivobject_05_subdivider_num = this->subdivobject_05_subdivider_getPatternNumerator(step);
    } else {
        this->subdivobject_05_subdivider_num = 1;

        if ((bool)(this->subdivobject_05_subdivider_prob->length) && (bool)(!(bool)(decided))) {
            number index = (number)(this->safemod(step, this->subdivobject_05_subdivider_prob->length));
            pv = this->subdivobject_05_subdivider_prob[(Index)index];

            if (pv == 0) {
                return false;
            }

            if (pv == 1) {
                return true;
            }

            return this->random(0, 1) < pv;
        } else if ((bool)(!(bool)(decided))) {
            result = true;
        }
    }

    return result;
}

bool subdivobject_05_subdivider_detectReset(number v, number p, number p2) {
    if (p2 < p && v < p && p - v > 0.25) {
        return true;
    }

    if (p2 > p && v > p && v - p > 0.25) {
        return true;
    }

    return false;
}

void subdivobject_05_subdivider_resolveSync() {
    if (this->subdivobject_05_subdivider_nextdiv != this->subdivobject_05_subdivider_div) {
        this->subdivobject_05_subdivider_div = this->subdivobject_05_subdivider_nextdiv;

        if (this->subdivobject_05_subdivider_pattern->length > 0) {
            this->subdivobject_05_subdivider_rebuild();
        }
    }
}

array<number, 2> subdivobject_05_subdivider_next(
    number x,
    number divarg,
    Int syncupdate,
    list prob,
    list pattern,
    bool lockprob,
    bool silentmode
) {
    this->subdivobject_05_subdivider_lockprob = lockprob;
    this->subdivobject_05_subdivider_setProb(prob);
    this->subdivobject_05_subdivider_setPattern(pattern);
    divarg = (divarg > 16384 ? 16384 : (divarg < 1 ? 1 : divarg));
    this->subdivobject_05_subdivider_setDiv(divarg);

    if (syncupdate == 0) {
        this->subdivobject_05_subdivider_resolveSync();
    }

    number div = (number)(this->subdivobject_05_subdivider_div);

    if (this->subdivobject_05_subdivider_posstep >= div) {
        this->subdivobject_05_subdivider_posstep = 0;
    }

    bool steppedalready = false;
    number val = (number)(x);

    if ((bool)(this->subdivobject_05_subdivider_detectReset(
        val,
        this->subdivobject_05_subdivider_prev,
        this->subdivobject_05_subdivider_prev2
    ))) {
        this->subdivobject_05_subdivider_posstep = 0;

        if (syncupdate > 0) {
            this->subdivobject_05_subdivider_resolveSync();
            div = this->subdivobject_05_subdivider_div;
        }

        if ((bool)(this->subdivobject_05_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_05_subdivider_lockprob))) {
            if (this->subdivobject_05_subdivider_probstep >= this->subdivobject_05_subdivider_prob->length) {
                this->subdivobject_05_subdivider_probstep = 0;
            }

            steppedalready = true;
        }

        this->subdivobject_05_subdivider_playing = this->subdivobject_05_subdivider_isPlaying(this->subdivobject_05_subdivider_posstep);
    }

    this->subdivobject_05_subdivider_prev2 = this->subdivobject_05_subdivider_prev;
    this->subdivobject_05_subdivider_prev = val;
    val *= div;
    this->subdivobject_05_subdivider_nextposstep = trunc(val);
    val -= this->subdivobject_05_subdivider_patternphase;
    val /= this->subdivobject_05_subdivider_num;
    this->subdivobject_05_subdivider_nextoutstep = trunc(val);

    if (this->subdivobject_05_subdivider_nextoutstep != this->subdivobject_05_subdivider_outstep) {
        if (syncupdate == 1) {
            this->subdivobject_05_subdivider_resolveSync();
            div = this->subdivobject_05_subdivider_div;
        }

        this->subdivobject_05_subdivider_outstep = this->subdivobject_05_subdivider_nextoutstep;

        if ((bool)(this->subdivobject_05_subdivider_prob->length) && (bool)(!(bool)(this->subdivobject_05_subdivider_lockprob)) && (bool)(!(bool)(steppedalready))) {
            this->subdivobject_05_subdivider_probstep += 1;

            if (this->subdivobject_05_subdivider_probstep >= this->subdivobject_05_subdivider_prob->length) {
                this->subdivobject_05_subdivider_probstep = 0;
            }
        }
    }

    if (this->subdivobject_05_subdivider_nextposstep != this->subdivobject_05_subdivider_posstep) {
        this->subdivobject_05_subdivider_posstep = this->subdivobject_05_subdivider_nextposstep;
        this->subdivobject_05_subdivider_playing = this->subdivobject_05_subdivider_isPlaying(this->subdivobject_05_subdivider_posstep);
    }

    number out1; number out2;

    if ((bool)(this->subdivobject_05_subdivider_playing)) {
        out1 = val - this->subdivobject_05_subdivider_nextoutstep;
        out2 = this->subdivobject_05_subdivider_nextoutstep;
        this->subdivobject_05_subdivider_lastoutstep = this->subdivobject_05_subdivider_nextoutstep;
    } else {
        out1 = 0.0;
        out2 = (silentmode == 0 ? this->subdivobject_05_subdivider_lastoutstep : -1);
    }

    return {out1, out2};
}

void subdivobject_05_dspsetup(bool force) {
    if ((bool)(this->subdivobject_05_setupDone) && (bool)(!(bool)(force)))
        return;

    this->subdivobject_05_setupDone = true;
    this->subdivobject_05_subdivider_dspsetup();
}

void phasor_05_dspsetup(bool force) {
    if ((bool)(this->phasor_05_setupDone) && (bool)(!(bool)(force)))
        return;

    this->phasor_05_conv = (number)1 / this->samplerate();
    this->phasor_05_setupDone = true;
}

void delta_tilde_01_reset() {
    this->delta_tilde_01_prev = 0;
}

void delta_tilde_01_dspsetup(bool force) {
    if ((bool)(this->delta_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->delta_tilde_01_reset();
    this->delta_tilde_01_setupDone = true;
}

void noise_tilde_01_init() {
    this->noise_tilde_01_reset();
}

void noise_tilde_01_reset() {
    xoshiro_reset(
        systemticks() + this->voice() + this->random(0, 10000),
        this->noise_tilde_01_state
    );
}

void timevalue_01_sendValue() {
    {
        {
            {
                {
                    {
                        {
                            this->timevalue_01_out_set(this->tickstohz(1920));
                        }
                    }
                }
            }
        }
    }
}

void timevalue_01_onTempoChanged(number tempo) {
    RNBO_UNUSED(tempo);

    {
        this->timevalue_01_sendValue();
    }
}

void timevalue_01_onSampleRateChanged(number ) {}

void timevalue_01_onTimeSignatureChanged(number , number ) {}

void timevalue_02_sendValue() {
    {
        {
            {
                {
                    {
                        {
                            this->timevalue_02_out_set(this->tickstohz(1920));
                        }
                    }
                }
            }
        }
    }
}

void timevalue_02_onTempoChanged(number tempo) {
    RNBO_UNUSED(tempo);

    {
        this->timevalue_02_sendValue();
    }
}

void timevalue_02_onSampleRateChanged(number ) {}

void timevalue_02_onTimeSignatureChanged(number , number ) {}

void timevalue_03_sendValue() {
    {
        {
            {
                {
                    {
                        {
                            this->timevalue_03_out_set(this->tickstohz(1920));
                        }
                    }
                }
            }
        }
    }
}

void timevalue_03_onTempoChanged(number tempo) {
    RNBO_UNUSED(tempo);

    {
        this->timevalue_03_sendValue();
    }
}

void timevalue_03_onSampleRateChanged(number ) {}

void timevalue_03_onTimeSignatureChanged(number , number ) {}

void timevalue_04_sendValue() {
    {
        {
            {
                {
                    {
                        {
                            this->timevalue_04_out_set(this->tickstohz(1920));
                        }
                    }
                }
            }
        }
    }
}

void timevalue_04_onTempoChanged(number tempo) {
    RNBO_UNUSED(tempo);

    {
        this->timevalue_04_sendValue();
    }
}

void timevalue_04_onSampleRateChanged(number ) {}

void timevalue_04_onTimeSignatureChanged(number , number ) {}

void timevalue_05_sendValue() {
    {
        {
            {
                {
                    {
                        {
                            this->timevalue_05_out_set(this->tickstohz(1920));
                        }
                    }
                }
            }
        }
    }
}

void timevalue_05_onTempoChanged(number tempo) {
    RNBO_UNUSED(tempo);

    {
        this->timevalue_05_sendValue();
    }
}

void timevalue_05_onSampleRateChanged(number ) {}

void timevalue_05_onTimeSignatureChanged(number , number ) {}

Index globaltransport_getSampleOffset(MillisecondTime time) {
    return this->mstosamps(this->maximum(0, time - this->getEngine()->getCurrentTime()));
}

number globaltransport_getTempoAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_tempo[(Index)sampleOffset] : this->globaltransport_lastTempo);
}

number globaltransport_getStateAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_state[(Index)sampleOffset] : this->globaltransport_lastState);
}

number globaltransport_getState(MillisecondTime time) {
    return this->globaltransport_getStateAtSample(this->globaltransport_getSampleOffset(time));
}

number globaltransport_getTempo(MillisecondTime time) {
    return this->globaltransport_getTempoAtSample(this->globaltransport_getSampleOffset(time));
}

number globaltransport_getBeatTime(MillisecondTime time) {
    number i = 2;

    while (i < this->globaltransport_beatTimeChanges->length && this->globaltransport_beatTimeChanges[(Index)(i + 1)] <= time) {
        i += 2;
    }

    i -= 2;
    number beatTimeBase = this->globaltransport_beatTimeChanges[(Index)i];

    if (this->globaltransport_getState(time) == 0)
        return beatTimeBase;

    number beatTimeBaseMsTime = this->globaltransport_beatTimeChanges[(Index)(i + 1)];
    number diff = time - beatTimeBaseMsTime;
    number diffInBeats = diff * this->globaltransport_getTempo(time) * 0.008 / (number)480;
    return beatTimeBase + diffInBeats;
}

bool globaltransport_setTempo(MillisecondTime time, number tempo, bool notify) {
    if ((bool)(notify)) {
        this->processTempoEvent(time, tempo);
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getTempoAtSample((SampleIndex)(offset)) != tempo) {
            this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
            this->globaltransport_beatTimeChanges->push(time);
            fillSignal(this->globaltransport_tempo, this->vs, tempo, offset);
            this->globaltransport_lastTempo = tempo;
            this->globaltransport_tempoNeedsReset = true;
            return true;
        }
    }

    return false;
}

bool globaltransport_setState(MillisecondTime time, number state, bool notify) {
    if ((bool)(notify)) {
        this->processTransportEvent(time, TransportState(state));
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getStateAtSample(offset) != state) {
            fillSignal(this->globaltransport_state, this->vs, state, offset);
            this->globaltransport_lastState = TransportState(state);
            this->globaltransport_stateNeedsReset = true;

            if (state == 0) {
                this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
                this->globaltransport_beatTimeChanges->push(time);
            }

            return true;
        }
    }

    return false;
}

bool globaltransport_setBeatTime(MillisecondTime time, number beattime, bool notify) {
    if ((bool)(notify)) {
        this->processBeatTimeEvent(time, beattime);
        this->globaltransport_notify = true;
        return false;
    } else {
        bool beatTimeHasChanged = false;
        float oldBeatTime = (float)(this->globaltransport_getBeatTime(time));
        float newBeatTime = (float)(beattime);

        if (oldBeatTime != newBeatTime) {
            beatTimeHasChanged = true;
        }

        this->globaltransport_beatTimeChanges->push(beattime);
        this->globaltransport_beatTimeChanges->push(time);
        return beatTimeHasChanged;
    }
}

number globaltransport_getBeatTimeAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getBeatTime(this->getEngine()->getCurrentTime() + msOffset);
}

array<number, 2> globaltransport_getTimeSignature(MillisecondTime time) {
    number i = 3;

    while (i < this->globaltransport_timeSignatureChanges->length && this->globaltransport_timeSignatureChanges[(Index)(i + 2)] <= time) {
        i += 3;
    }

    i -= 3;

    return {
        this->globaltransport_timeSignatureChanges[(Index)i],
        this->globaltransport_timeSignatureChanges[(Index)(i + 1)]
    };
}

array<number, 2> globaltransport_getTimeSignatureAtSample(SampleIndex sampleOffset) {
    MillisecondTime msOffset = (MillisecondTime)(this->sampstoms(sampleOffset));
    return this->globaltransport_getTimeSignature(this->getEngine()->getCurrentTime() + msOffset);
}

bool globaltransport_setTimeSignature(MillisecondTime time, number numerator, number denominator, bool notify) {
    if ((bool)(notify)) {
        this->processTimeSignatureEvent(time, (int)(numerator), (int)(denominator));
        this->globaltransport_notify = true;
    } else {
        array<number, 2> currentSig = this->globaltransport_getTimeSignature(time);

        if (currentSig[0] != numerator || currentSig[1] != denominator) {
            this->globaltransport_timeSignatureChanges->push(numerator);
            this->globaltransport_timeSignatureChanges->push(denominator);
            this->globaltransport_timeSignatureChanges->push(time);
            return true;
        }
    }

    return false;
}

void globaltransport_advance() {
    if ((bool)(this->globaltransport_tempoNeedsReset)) {
        fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
        this->globaltransport_tempoNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTempoEvent(this->globaltransport_lastTempo);
        }
    }

    if ((bool)(this->globaltransport_stateNeedsReset)) {
        fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
        this->globaltransport_stateNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTransportEvent(TransportState(this->globaltransport_lastState));
        }
    }

    if (this->globaltransport_beatTimeChanges->length > 2) {
        this->globaltransport_beatTimeChanges[0] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 2)];
        this->globaltransport_beatTimeChanges[1] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 1)];
        this->globaltransport_beatTimeChanges->length = 2;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendBeatTimeEvent(this->globaltransport_beatTimeChanges[0]);
        }
    }

    if (this->globaltransport_timeSignatureChanges->length > 3) {
        this->globaltransport_timeSignatureChanges[0] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 3)];
        this->globaltransport_timeSignatureChanges[1] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 2)];
        this->globaltransport_timeSignatureChanges[2] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 1)];
        this->globaltransport_timeSignatureChanges->length = 3;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTimeSignatureEvent(
                (int)(this->globaltransport_timeSignatureChanges[0]),
                (int)(this->globaltransport_timeSignatureChanges[1])
            );
        }
    }

    this->globaltransport_notify = false;
}

void globaltransport_dspsetup(bool force) {
    if ((bool)(this->globaltransport_setupDone) && (bool)(!(bool)(force)))
        return;

    fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
    this->globaltransport_tempoNeedsReset = false;
    fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
    this->globaltransport_stateNeedsReset = false;
    this->globaltransport_setupDone = true;
}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    scale_tilde_01_x = 0;
    scale_tilde_01_lowin = -1;
    scale_tilde_01_hiin = 1;
    scale_tilde_01_lowout = 0;
    scale_tilde_01_highout = 1;
    scale_tilde_01_pow = 1;
    subdivobject_01_syncupdate = 0;
    subdivobject_01_lockprob = 1;
    subdivobject_01_silentmode = 0;
    subdivobject_01_div = 1;
    subdivobject_01_input_number = 0;
    subdivobject_01_stepmessage = 0;
    phasor_01_freq = 0;
    dspexpr_01_in1 = 0;
    dspexpr_02_in1 = 0;
    dspexpr_02_in2 = 6.28;
    subdivobject_02_syncupdate = 0;
    subdivobject_02_lockprob = 1;
    subdivobject_02_silentmode = 0;
    subdivobject_02_div = 1;
    subdivobject_02_input_number = 0;
    subdivobject_02_stepmessage = 0;
    phasor_02_freq = 0;
    dspexpr_03_in1 = 0;
    dspexpr_03_in2 = 0.5;
    subdivobject_03_syncupdate = 0;
    subdivobject_03_lockprob = 1;
    subdivobject_03_silentmode = 0;
    subdivobject_03_div = 1;
    subdivobject_03_input_number = 0;
    subdivobject_03_stepmessage = 0;
    phasor_03_freq = 0;
    param_01_value = 1;
    dspexpr_04_in1 = 0;
    dspexpr_05_in1 = 0;
    dspexpr_05_in2 = 1;
    dspexpr_06_in1 = 0;
    dspexpr_06_in2 = 2;
    subdivobject_04_syncupdate = 0;
    subdivobject_04_lockprob = 1;
    subdivobject_04_silentmode = 0;
    subdivobject_04_div = 1;
    subdivobject_04_input_number = 0;
    subdivobject_04_stepmessage = 0;
    phasor_04_freq = 0;
    wrap_tilde_01_x = 0;
    wrap_tilde_01_low = 0;
    wrap_tilde_01_high = 1;
    subdivobject_05_syncupdate = 0;
    subdivobject_05_lockprob = 1;
    subdivobject_05_silentmode = 0;
    subdivobject_05_div = 1;
    subdivobject_05_input_number = 0;
    subdivobject_05_stepmessage = 0;
    dspexpr_07_in1 = 0;
    dspexpr_07_in2 = 0;
    dspexpr_08_in1 = 0;
    dspexpr_08_in2 = 0;
    phasor_05_freq = 0;
    delta_tilde_01_x = 0;
    accum_tilde_01_x = 0;
    accum_tilde_01_reset = 0;
    dspexpr_09_in1 = 0;
    dspexpr_09_in2 = 0.05;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    signals[1] = nullptr;
    signals[2] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    subdivobject_01_subdivider_div = 1;
    subdivobject_01_subdivider_nextdiv = 1;
    subdivobject_01_subdivider_posstep = 0;
    subdivobject_01_subdivider_nextposstep = 0;
    subdivobject_01_subdivider_outstep = 0;
    subdivobject_01_subdivider_nextoutstep = 0;
    subdivobject_01_subdivider_prev = 0;
    subdivobject_01_subdivider_prev2 = 0;
    subdivobject_01_subdivider_playing = true;
    subdivobject_01_subdivider_lockprob = true;
    subdivobject_01_setupDone = false;
    phasor_01_sigbuf = nullptr;
    phasor_01_lastLockedPhase = 0;
    phasor_01_conv = 0;
    phasor_01_setupDone = false;
    subdivobject_02_subdivider_div = 1;
    subdivobject_02_subdivider_nextdiv = 1;
    subdivobject_02_subdivider_posstep = 0;
    subdivobject_02_subdivider_nextposstep = 0;
    subdivobject_02_subdivider_outstep = 0;
    subdivobject_02_subdivider_nextoutstep = 0;
    subdivobject_02_subdivider_prev = 0;
    subdivobject_02_subdivider_prev2 = 0;
    subdivobject_02_subdivider_playing = true;
    subdivobject_02_subdivider_lockprob = true;
    subdivobject_02_setupDone = false;
    phasor_02_sigbuf = nullptr;
    phasor_02_lastLockedPhase = 0;
    phasor_02_conv = 0;
    phasor_02_setupDone = false;
    subdivobject_03_subdivider_div = 1;
    subdivobject_03_subdivider_nextdiv = 1;
    subdivobject_03_subdivider_posstep = 0;
    subdivobject_03_subdivider_nextposstep = 0;
    subdivobject_03_subdivider_outstep = 0;
    subdivobject_03_subdivider_nextoutstep = 0;
    subdivobject_03_subdivider_prev = 0;
    subdivobject_03_subdivider_prev2 = 0;
    subdivobject_03_subdivider_playing = true;
    subdivobject_03_subdivider_lockprob = true;
    subdivobject_03_setupDone = false;
    phasor_03_sigbuf = nullptr;
    phasor_03_lastLockedPhase = 0;
    phasor_03_conv = 0;
    phasor_03_setupDone = false;
    param_01_lastValue = 0;
    subdivobject_04_subdivider_div = 1;
    subdivobject_04_subdivider_nextdiv = 1;
    subdivobject_04_subdivider_posstep = 0;
    subdivobject_04_subdivider_nextposstep = 0;
    subdivobject_04_subdivider_outstep = 0;
    subdivobject_04_subdivider_nextoutstep = 0;
    subdivobject_04_subdivider_prev = 0;
    subdivobject_04_subdivider_prev2 = 0;
    subdivobject_04_subdivider_playing = true;
    subdivobject_04_subdivider_lockprob = true;
    subdivobject_04_setupDone = false;
    phasor_04_sigbuf = nullptr;
    phasor_04_lastLockedPhase = 0;
    phasor_04_conv = 0;
    phasor_04_setupDone = false;
    subdivobject_05_subdivider_div = 1;
    subdivobject_05_subdivider_nextdiv = 1;
    subdivobject_05_subdivider_posstep = 0;
    subdivobject_05_subdivider_nextposstep = 0;
    subdivobject_05_subdivider_outstep = 0;
    subdivobject_05_subdivider_nextoutstep = 0;
    subdivobject_05_subdivider_prev = 0;
    subdivobject_05_subdivider_prev2 = 0;
    subdivobject_05_subdivider_playing = true;
    subdivobject_05_subdivider_lockprob = true;
    subdivobject_05_setupDone = false;
    phasor_05_sigbuf = nullptr;
    phasor_05_lastLockedPhase = 0;
    phasor_05_conv = 0;
    phasor_05_setupDone = false;
    delta_tilde_01_prev = 0;
    delta_tilde_01_setupDone = false;
    accum_tilde_01_value = 0;
    globaltransport_tempo = nullptr;
    globaltransport_tempoNeedsReset = false;
    globaltransport_lastTempo = 120;
    globaltransport_state = nullptr;
    globaltransport_stateNeedsReset = false;
    globaltransport_lastState = 0;
    globaltransport_beatTimeChanges = { 0, 0 };
    globaltransport_timeSignatureChanges = { 4, 4, 0 };
    globaltransport_notify = false;
    globaltransport_setupDone = false;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number scale_tilde_01_x;
    number scale_tilde_01_lowin;
    number scale_tilde_01_hiin;
    number scale_tilde_01_lowout;
    number scale_tilde_01_highout;
    number scale_tilde_01_pow;
    Int subdivobject_01_syncupdate;
    list subdivobject_01_prob_list;
    list subdivobject_01_pattern_list;
    number subdivobject_01_lockprob;
    number subdivobject_01_silentmode;
    number subdivobject_01_div;
    number subdivobject_01_input_number;
    number subdivobject_01_stepmessage;
    number phasor_01_freq;
    number dspexpr_01_in1;
    number dspexpr_02_in1;
    number dspexpr_02_in2;
    Int subdivobject_02_syncupdate;
    list subdivobject_02_prob_list;
    list subdivobject_02_pattern_list;
    number subdivobject_02_lockprob;
    number subdivobject_02_silentmode;
    number subdivobject_02_div;
    number subdivobject_02_input_number;
    number subdivobject_02_stepmessage;
    number phasor_02_freq;
    number dspexpr_03_in1;
    number dspexpr_03_in2;
    Int subdivobject_03_syncupdate;
    list subdivobject_03_prob_list;
    list subdivobject_03_pattern_list;
    number subdivobject_03_lockprob;
    number subdivobject_03_silentmode;
    number subdivobject_03_div;
    number subdivobject_03_input_number;
    number subdivobject_03_stepmessage;
    number phasor_03_freq;
    number param_01_value;
    number dspexpr_04_in1;
    number dspexpr_05_in1;
    number dspexpr_05_in2;
    number dspexpr_06_in1;
    number dspexpr_06_in2;
    Int subdivobject_04_syncupdate;
    list subdivobject_04_prob_list;
    list subdivobject_04_pattern_list;
    number subdivobject_04_lockprob;
    number subdivobject_04_silentmode;
    number subdivobject_04_div;
    number subdivobject_04_input_number;
    number subdivobject_04_stepmessage;
    number phasor_04_freq;
    number wrap_tilde_01_x;
    number wrap_tilde_01_low;
    number wrap_tilde_01_high;
    Int subdivobject_05_syncupdate;
    list subdivobject_05_prob_list;
    list subdivobject_05_pattern_list;
    number subdivobject_05_lockprob;
    number subdivobject_05_silentmode;
    number subdivobject_05_div;
    number subdivobject_05_input_number;
    number subdivobject_05_stepmessage;
    number dspexpr_07_in1;
    number dspexpr_07_in2;
    number dspexpr_08_in1;
    number dspexpr_08_in2;
    number phasor_05_freq;
    number delta_tilde_01_x;
    number accum_tilde_01_x;
    number accum_tilde_01_reset;
    number dspexpr_09_in1;
    number dspexpr_09_in2;
    MillisecondTime _currentTime;
    UInt64 audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[3];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    number subdivobject_01_subdivider_div;
    number subdivobject_01_subdivider_nextdiv;
    number subdivobject_01_subdivider_posstep;
    number subdivobject_01_subdivider_nextposstep;
    number subdivobject_01_subdivider_outstep;
    number subdivobject_01_subdivider_nextoutstep;
    number subdivobject_01_subdivider_lastoutstep;
    number subdivobject_01_subdivider_prev;
    number subdivobject_01_subdivider_prev2;
    list subdivobject_01_subdivider_p2;
    list subdivobject_01_subdivider_synco;
    list subdivobject_01_subdivider_p2prob;
    list subdivobject_01_subdivider_pattern;
    list subdivobject_01_subdivider_prob;
    number subdivobject_01_subdivider_p2length;
    bool subdivobject_01_subdivider_playing;
    number subdivobject_01_subdivider_num;
    number subdivobject_01_subdivider_patternphase;
    number subdivobject_01_subdivider_patternstep;
    number subdivobject_01_subdivider_probstep;
    bool subdivobject_01_subdivider_lockprob;
    bool subdivobject_01_setupDone;
    signal phasor_01_sigbuf;
    number phasor_01_lastLockedPhase;
    number phasor_01_conv;
    bool phasor_01_setupDone;
    number subdivobject_02_subdivider_div;
    number subdivobject_02_subdivider_nextdiv;
    number subdivobject_02_subdivider_posstep;
    number subdivobject_02_subdivider_nextposstep;
    number subdivobject_02_subdivider_outstep;
    number subdivobject_02_subdivider_nextoutstep;
    number subdivobject_02_subdivider_lastoutstep;
    number subdivobject_02_subdivider_prev;
    number subdivobject_02_subdivider_prev2;
    list subdivobject_02_subdivider_p2;
    list subdivobject_02_subdivider_synco;
    list subdivobject_02_subdivider_p2prob;
    list subdivobject_02_subdivider_pattern;
    list subdivobject_02_subdivider_prob;
    number subdivobject_02_subdivider_p2length;
    bool subdivobject_02_subdivider_playing;
    number subdivobject_02_subdivider_num;
    number subdivobject_02_subdivider_patternphase;
    number subdivobject_02_subdivider_patternstep;
    number subdivobject_02_subdivider_probstep;
    bool subdivobject_02_subdivider_lockprob;
    bool subdivobject_02_setupDone;
    signal phasor_02_sigbuf;
    number phasor_02_lastLockedPhase;
    number phasor_02_conv;
    bool phasor_02_setupDone;
    number subdivobject_03_subdivider_div;
    number subdivobject_03_subdivider_nextdiv;
    number subdivobject_03_subdivider_posstep;
    number subdivobject_03_subdivider_nextposstep;
    number subdivobject_03_subdivider_outstep;
    number subdivobject_03_subdivider_nextoutstep;
    number subdivobject_03_subdivider_lastoutstep;
    number subdivobject_03_subdivider_prev;
    number subdivobject_03_subdivider_prev2;
    list subdivobject_03_subdivider_p2;
    list subdivobject_03_subdivider_synco;
    list subdivobject_03_subdivider_p2prob;
    list subdivobject_03_subdivider_pattern;
    list subdivobject_03_subdivider_prob;
    number subdivobject_03_subdivider_p2length;
    bool subdivobject_03_subdivider_playing;
    number subdivobject_03_subdivider_num;
    number subdivobject_03_subdivider_patternphase;
    number subdivobject_03_subdivider_patternstep;
    number subdivobject_03_subdivider_probstep;
    bool subdivobject_03_subdivider_lockprob;
    bool subdivobject_03_setupDone;
    signal phasor_03_sigbuf;
    number phasor_03_lastLockedPhase;
    number phasor_03_conv;
    bool phasor_03_setupDone;
    number param_01_lastValue;
    number subdivobject_04_subdivider_div;
    number subdivobject_04_subdivider_nextdiv;
    number subdivobject_04_subdivider_posstep;
    number subdivobject_04_subdivider_nextposstep;
    number subdivobject_04_subdivider_outstep;
    number subdivobject_04_subdivider_nextoutstep;
    number subdivobject_04_subdivider_lastoutstep;
    number subdivobject_04_subdivider_prev;
    number subdivobject_04_subdivider_prev2;
    list subdivobject_04_subdivider_p2;
    list subdivobject_04_subdivider_synco;
    list subdivobject_04_subdivider_p2prob;
    list subdivobject_04_subdivider_pattern;
    list subdivobject_04_subdivider_prob;
    number subdivobject_04_subdivider_p2length;
    bool subdivobject_04_subdivider_playing;
    number subdivobject_04_subdivider_num;
    number subdivobject_04_subdivider_patternphase;
    number subdivobject_04_subdivider_patternstep;
    number subdivobject_04_subdivider_probstep;
    bool subdivobject_04_subdivider_lockprob;
    bool subdivobject_04_setupDone;
    signal phasor_04_sigbuf;
    number phasor_04_lastLockedPhase;
    number phasor_04_conv;
    bool phasor_04_setupDone;
    number subdivobject_05_subdivider_div;
    number subdivobject_05_subdivider_nextdiv;
    number subdivobject_05_subdivider_posstep;
    number subdivobject_05_subdivider_nextposstep;
    number subdivobject_05_subdivider_outstep;
    number subdivobject_05_subdivider_nextoutstep;
    number subdivobject_05_subdivider_lastoutstep;
    number subdivobject_05_subdivider_prev;
    number subdivobject_05_subdivider_prev2;
    list subdivobject_05_subdivider_p2;
    list subdivobject_05_subdivider_synco;
    list subdivobject_05_subdivider_p2prob;
    list subdivobject_05_subdivider_pattern;
    list subdivobject_05_subdivider_prob;
    number subdivobject_05_subdivider_p2length;
    bool subdivobject_05_subdivider_playing;
    number subdivobject_05_subdivider_num;
    number subdivobject_05_subdivider_patternphase;
    number subdivobject_05_subdivider_patternstep;
    number subdivobject_05_subdivider_probstep;
    bool subdivobject_05_subdivider_lockprob;
    bool subdivobject_05_setupDone;
    signal phasor_05_sigbuf;
    number phasor_05_lastLockedPhase;
    number phasor_05_conv;
    bool phasor_05_setupDone;
    number delta_tilde_01_prev;
    bool delta_tilde_01_setupDone;
    number accum_tilde_01_value;
    UInt noise_tilde_01_state[4] = { };
    signal globaltransport_tempo;
    bool globaltransport_tempoNeedsReset;
    number globaltransport_lastTempo;
    signal globaltransport_state;
    bool globaltransport_stateNeedsReset;
    number globaltransport_lastState;
    list globaltransport_beatTimeChanges;
    list globaltransport_timeSignatureChanges;
    bool globaltransport_notify;
    bool globaltransport_setupDone;
    number stackprotect_count;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* createsynclfo()
{
    return new synclfo();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr synclfoFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return createsynclfo;
}

} // end RNBO namespace

