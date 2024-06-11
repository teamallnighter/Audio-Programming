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

class yoi : public PatcherInterfaceImpl {
public:

yoi()
{
}

~yoi()
{
}

yoi* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
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

number samplerate() const {
    return this->sr;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
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
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);

    this->svf_tilde_01_perform(
        in1,
        this->svf_tilde_01_freq,
        this->svf_tilde_01_q,
        this->signals[0],
        this->dummyBuffer,
        this->signals[1],
        this->dummyBuffer,
        n
    );

    this->selector_01_perform(
        this->selector_01_onoff,
        this->signals[0],
        this->signals[1],
        this->signals[2],
        n
    );

    this->downsamp_tilde_01_perform(this->signals[2], this->downsamp_tilde_01_downSample, this->signals[1], n);
    this->limi_01_perform(this->signals[1], this->signals[1], out1, out2, n);
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

    this->svf_tilde_01_dspsetup(forceDSPSetup);
    this->limi_01_dspsetup(forceDSPSetup);
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

void onSampleRateChanged(double ) {}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "filter_select"));
    this->param_02_getPresetValue(getSubState(preset, "downsample"));
    this->param_03_getPresetValue(getSubState(preset, "filter_frequency"));
    this->param_04_getPresetValue(getSubState(preset, "filter_q"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "filter_select"));
    this->param_02_setPresetValue(getSubState(preset, "downsample"));
    this->param_03_setPresetValue(getSubState(preset, "filter_frequency"));
    this->param_04_setPresetValue(getSubState(preset, "filter_q"));
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
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
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
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
    return 4;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "filter_select";
        }
    case 1:
        {
        return "downsample";
        }
    case 2:
        {
        return "filter_frequency";
        }
    case 3:
        {
        return "filter_q";
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
        return "filter_select";
        }
    case 1:
        {
        return "downsample";
        }
    case 2:
        {
        return "filter_frequency";
        }
    case 3:
        {
        return "filter_q";
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
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 2;
            static const char * eVal0[] = {"lowpass", "bandpass"};
            info->enumValues = eVal0;
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
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 20;
            info->min = 1;
            info->max = 40;
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
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 250;
            info->min = 20;
            info->max = 2500;
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
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.5;
            info->min = 0;
            info->max = 1;
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
    case 3:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);
            return normalizedValue;
        }
        }
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);

            {
                normalizedValue = this->applyStepsToNormalizedParameterValue(normalizedValue, 2);
            }

            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < 1 ? 1 : (value > 40 ? 40 : value));
            ParameterValue normalizedValue = (value - 1) / (40 - 1);
            return normalizedValue;
        }
        }
    case 2:
        {
        {
            value = (value < 20 ? 20 : (value > 2500 ? 2500 : value));
            ParameterValue normalizedValue = (value - 20) / (2500 - 20);
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
    case 3:
        {
        {
            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 0:
        {
        {
            {
                value = this->applyStepsToNormalizedParameterValue(value, 2);
            }

            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 1:
        {
        {
            {
                return 1 + value * (40 - 1);
            }
        }
        }
    case 2:
        {
        {
            {
                return 20 + value * (2500 - 20);
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
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
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

void processClockEvent(MillisecondTime , ClockId , bool , ParameterValue ) {}

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

    this->expr_01_in1_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->downsamp_tilde_01_downSample_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->svf_tilde_01_freq_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        this->getEngine()->presetTouched();
        this->param_04_lastValue = this->param_04_value;
    }

    this->svf_tilde_01_q_set(v);
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
    return 1;
}

Index getNumOutputChannels() const {
    return 2;
}

void allocateDataRefs() {}

void initializeObjects() {}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    this->processParamInitEvents();
}

number param_01_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));

    {
        number oneStep = (number)1 / (number)1;
        number oneStepInv = (oneStep != 0 ? (number)1 / oneStep : 0);
        number numberOfSteps = rnbo_fround(v * oneStepInv * 1 / (number)1) * 1;
        v = numberOfSteps * oneStep;
    }

    return v;
}

void selector_01_onoff_set(number v) {
    this->selector_01_onoff = v;
}

void expr_01_out1_set(number v) {
    this->expr_01_out1 = v;
    this->selector_01_onoff_set(this->expr_01_out1);
}

void expr_01_in1_set(number in1) {
    this->expr_01_in1 = in1;
    this->expr_01_out1_set(this->expr_01_in1 + this->expr_01_in2);//#map:+_obj-6:1
}

number param_02_value_constrain(number v) const {
    v = (v > 40 ? 40 : (v < 1 ? 1 : v));
    return v;
}

void downsamp_tilde_01_downSample_set(number v) {
    this->downsamp_tilde_01_downSample = v;
}

number param_03_value_constrain(number v) const {
    v = (v > 2500 ? 2500 : (v < 20 ? 20 : v));
    return v;
}

void svf_tilde_01_freq_set(number v) {
    this->svf_tilde_01_freq = v;
}

number param_04_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void svf_tilde_01_q_set(number v) {
    this->svf_tilde_01_q = v;
}

void svf_tilde_01_perform(
    const Sample * x,
    number freq,
    number q,
    SampleValue * out1,
    SampleValue * out2,
    SampleValue * out3,
    SampleValue * out4,
    Index n
) {
    auto __svf_tilde_01_high = this->svf_tilde_01_high;
    auto __svf_tilde_01_notch = this->svf_tilde_01_notch;
    auto __svf_tilde_01_band = this->svf_tilde_01_band;
    auto __svf_tilde_01_low = this->svf_tilde_01_low;
    auto __svf_tilde_01__q = this->svf_tilde_01__q;
    auto __svf_tilde_01__freq = this->svf_tilde_01__freq;
    Index i;

    for (i = 0; i < n; i++) {
        if (this->svf_tilde_01_dFreq_next(freq) != 0) {
            __svf_tilde_01__freq = (freq > this->sr * 0.25 ? this->sr * 0.25 : (freq < 0.0 ? 0.0 : freq));
            __svf_tilde_01__freq = rnbo_sin(6.28318530717958647692 * __svf_tilde_01__freq / this->sr);
            __svf_tilde_01__freq = (__svf_tilde_01__freq > 0.99 ? 0.99 : (__svf_tilde_01__freq < 0.001 ? 0.001 : __svf_tilde_01__freq));
        }

        if (this->svf_tilde_01_dQ_next(q) != 0) {
            __svf_tilde_01__q = (1. - q) * 1.2;
            __svf_tilde_01__q = __svf_tilde_01__q * __svf_tilde_01__q;
            __svf_tilde_01__q = (__svf_tilde_01__q > 0.99 ? 0.99 : (__svf_tilde_01__q < 0.001 ? 0.001 : __svf_tilde_01__q));
        }

        __svf_tilde_01_low += __svf_tilde_01__freq * __svf_tilde_01_band;
        __svf_tilde_01_notch = x[(Index)i] - __svf_tilde_01__q * __svf_tilde_01_band;
        __svf_tilde_01_high = __svf_tilde_01_notch - __svf_tilde_01_low;
        __svf_tilde_01_band += __svf_tilde_01__freq * __svf_tilde_01_high - __svf_tilde_01_band * __svf_tilde_01_band * __svf_tilde_01_band * 0.0001;
        array<number, 4> tmp = {0, 0, 0, 0};
        tmp[0] = __svf_tilde_01_low;
        tmp[1] = __svf_tilde_01_high;
        tmp[2] = __svf_tilde_01_band;
        tmp[3] = __svf_tilde_01_notch;
        out1[(Index)i] = tmp[0];
        out2[(Index)i] = tmp[1];
        out3[(Index)i] = tmp[2];
        out4[(Index)i] = tmp[3];
    }

    this->svf_tilde_01__freq = __svf_tilde_01__freq;
    this->svf_tilde_01__q = __svf_tilde_01__q;
    this->svf_tilde_01_low = __svf_tilde_01_low;
    this->svf_tilde_01_band = __svf_tilde_01_band;
    this->svf_tilde_01_notch = __svf_tilde_01_notch;
    this->svf_tilde_01_high = __svf_tilde_01_high;
}

void selector_01_perform(
    number onoff,
    const SampleValue * in1,
    const SampleValue * in2,
    SampleValue * out,
    Index n
) {
    Index i;

    for (i = 0; i < n; i++) {
        if (onoff >= 1 && onoff < 2)
            out[(Index)i] = in1[(Index)i];
        else if (onoff >= 2 && onoff < 3)
            out[(Index)i] = in2[(Index)i];
        else
            out[(Index)i] = 0;
    }
}

void downsamp_tilde_01_perform(const Sample * x, number downSample, SampleValue * out1, Index n) {
    auto __downsamp_tilde_01_hold = this->downsamp_tilde_01_hold;
    auto __downsamp_tilde_01_current = this->downsamp_tilde_01_current;
    auto __downsamp_tilde_01_lastDownSample = this->downsamp_tilde_01_lastDownSample;
    Index i;

    for (i = 0; i < n; i++) {
        if (downSample != __downsamp_tilde_01_lastDownSample) {
            __downsamp_tilde_01_lastDownSample = this->maximum(1.0, rnbo_abs(downSample));

            if (__downsamp_tilde_01_lastDownSample < __downsamp_tilde_01_current) {
                __downsamp_tilde_01_current = __downsamp_tilde_01_lastDownSample;
            }
        }

        __downsamp_tilde_01_current++;

        if (__downsamp_tilde_01_current >= __downsamp_tilde_01_lastDownSample) {
            __downsamp_tilde_01_current -= __downsamp_tilde_01_lastDownSample;
            __downsamp_tilde_01_hold = x[(Index)i];
        }

        out1[(Index)i] = __downsamp_tilde_01_hold;
    }

    this->downsamp_tilde_01_lastDownSample = __downsamp_tilde_01_lastDownSample;
    this->downsamp_tilde_01_current = __downsamp_tilde_01_current;
    this->downsamp_tilde_01_hold = __downsamp_tilde_01_hold;
}

void limi_01_perform(
    const SampleValue * input1,
    const SampleValue * input2,
    SampleValue * output1,
    SampleValue * output2,
    Index n
) {
    RNBO_UNUSED(output2);
    RNBO_UNUSED(output1);
    RNBO_UNUSED(input2);
    RNBO_UNUSED(input1);
    auto __limi_01_lookaheadInv = this->limi_01_lookaheadInv;
    auto __limi_01_threshold = this->limi_01_threshold;
    auto __limi_01_lookahead = this->limi_01_lookahead;
    auto __limi_01_recover = this->limi_01_recover;
    auto __limi_01_last = this->limi_01_last;
    auto __limi_01_postamp = this->limi_01_postamp;
    auto __limi_01_lookaheadIndex = this->limi_01_lookaheadIndex;
    auto __limi_01_preamp = this->limi_01_preamp;
    auto __limi_01_dcblock = this->limi_01_dcblock;
    auto __limi_01_bypass = this->limi_01_bypass;
    ConstSampleArray<2> input = {input1, input2};
    SampleArray<2> output = {output1, output2};

    if ((bool)(__limi_01_bypass)) {
        for (Index i = 0; i < n; i++) {
            for (Index j = 0; j < 2; j++) {
                output[(Index)j][(Index)i] = input[(Index)j][(Index)i];
            }
        }
    } else {
        number v;

        for (Index i = 0; i < n; i++) {
            number hotSample = 0;

            for (Index j = 0; j < 2; j++) {
                auto smps = input[(Index)j];
                v = ((bool)(__limi_01_dcblock) ? this->limi_01_dc_next(j, smps[(Index)i], 0.9997) : smps[(Index)i]);
                v *= __limi_01_preamp;
                this->limi_01_lookaheadBuffers[(Index)j][__limi_01_lookaheadIndex] = v * __limi_01_postamp;
                v = rnbo_fabs(v);

                if (v > hotSample)
                    hotSample = v;
            }

            {
                if (__limi_01_last > 0.01)
                    v = __limi_01_last + __limi_01_recover * __limi_01_last;
                else
                    v = __limi_01_last + __limi_01_recover;
            }

            if (v > 1)
                v = 1;

            this->limi_01_gainBuffer[__limi_01_lookaheadIndex] = v;
            int lookaheadPlayback = (int)(__limi_01_lookaheadIndex - (int)(__limi_01_lookahead));

            if (lookaheadPlayback < 0)
                lookaheadPlayback += (int)(__limi_01_lookahead);

            if (hotSample * v > __limi_01_threshold) {
                number newgain;
                number curgain = __limi_01_threshold / hotSample;
                number inc = __limi_01_threshold - curgain;
                number acc = 0.0;
                number flag = 0;

                for (Index j = 0; flag == 0 && j < (Index)(__limi_01_lookahead); j++) {
                    int k = (int)(__limi_01_lookaheadIndex - (int)(j));

                    if (k < 0)
                        k += (int)(__limi_01_lookahead);

                    {
                        newgain = curgain + inc * (acc * acc);
                    }

                    if (newgain < this->limi_01_gainBuffer[(Index)k])
                        this->limi_01_gainBuffer[(Index)k] = newgain;
                    else
                        flag = 1;

                    acc = acc + __limi_01_lookaheadInv;
                }
            }

            for (Index j = 0; j < 2; j++) {
                output[(Index)j][(Index)i] = this->limi_01_lookaheadBuffers[(Index)j][(Index)lookaheadPlayback] * this->limi_01_gainBuffer[(Index)lookaheadPlayback];
            }

            __limi_01_last = this->limi_01_gainBuffer[__limi_01_lookaheadIndex];
            __limi_01_lookaheadIndex++;

            if (__limi_01_lookaheadIndex >= __limi_01_lookahead)
                __limi_01_lookaheadIndex = 0;
        }
    }

    this->limi_01_lookaheadIndex = __limi_01_lookaheadIndex;
    this->limi_01_last = __limi_01_last;
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void limi_01_lookahead_setter(number v) {
    this->limi_01_lookahead = (v > 512 ? 512 : (v < 0 ? 0 : v));
    this->limi_01_lookaheadInv = (number)1 / this->limi_01_lookahead;
}

void limi_01_preamp_setter(number v) {
    this->limi_01_preamp = rnbo_pow(10., v * 0.05);
}

void limi_01_postamp_setter(number v) {
    this->limi_01_postamp = rnbo_pow(10., v * 0.05);
}

void limi_01_threshold_setter(number v) {
    this->limi_01_threshold = rnbo_pow(10., v * 0.05);
}

number limi_01_dc1_next(number x, number gain) {
    number y = x - this->limi_01_dc1_xm1 + this->limi_01_dc1_ym1 * gain;
    this->limi_01_dc1_xm1 = x;
    this->limi_01_dc1_ym1 = y;
    return y;
}

void limi_01_dc1_reset() {
    this->limi_01_dc1_xm1 = 0;
    this->limi_01_dc1_ym1 = 0;
}

void limi_01_dc1_dspsetup() {
    this->limi_01_dc1_reset();
}

number limi_01_dc2_next(number x, number gain) {
    number y = x - this->limi_01_dc2_xm1 + this->limi_01_dc2_ym1 * gain;
    this->limi_01_dc2_xm1 = x;
    this->limi_01_dc2_ym1 = y;
    return y;
}

void limi_01_dc2_reset() {
    this->limi_01_dc2_xm1 = 0;
    this->limi_01_dc2_ym1 = 0;
}

void limi_01_dc2_dspsetup() {
    this->limi_01_dc2_reset();
}

number limi_01_dc_next(Index i, number x, number gain) {
    switch ((int)i) {
    case 0:
        {
        return this->limi_01_dc1_next(x, gain);
        }
    default:
        {
        return this->limi_01_dc2_next(x, gain);
        }
    }

    return 0;
}

void limi_01_dc_reset(Index i) {
    switch ((int)i) {
    case 0:
        {
        return this->limi_01_dc1_reset();
        }
    default:
        {
        return this->limi_01_dc2_reset();
        }
    }
}

void limi_01_dc_dspsetup(Index i) {
    switch ((int)i) {
    case 0:
        {
        return this->limi_01_dc1_dspsetup();
        }
    default:
        {
        return this->limi_01_dc2_dspsetup();
        }
    }
}

void limi_01_reset() {
    this->limi_01_recover = (number)1000 / (this->limi_01_release * this->samplerate());

    {
        this->limi_01_recover *= 0.707;
    }
}

void limi_01_dspsetup(bool force) {
    if ((bool)(this->limi_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->limi_01_reset();
    this->limi_01_setupDone = true;
    this->limi_01_dc1_dspsetup();
    this->limi_01_dc2_dspsetup();
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

number svf_tilde_01_dFreq_next(number x) {
    number temp = (number)(x - this->svf_tilde_01_dFreq_prev);
    this->svf_tilde_01_dFreq_prev = x;
    return temp;
}

void svf_tilde_01_dFreq_dspsetup() {
    this->svf_tilde_01_dFreq_reset();
}

void svf_tilde_01_dFreq_reset() {
    this->svf_tilde_01_dFreq_prev = 0;
}

number svf_tilde_01_dQ_next(number x) {
    number temp = (number)(x - this->svf_tilde_01_dQ_prev);
    this->svf_tilde_01_dQ_prev = x;
    return temp;
}

void svf_tilde_01_dQ_dspsetup() {
    this->svf_tilde_01_dQ_reset();
}

void svf_tilde_01_dQ_reset() {
    this->svf_tilde_01_dQ_prev = 0;
}

void svf_tilde_01_reset() {
    this->svf_tilde_01_low = 0;
    this->svf_tilde_01_high = 0;
    this->svf_tilde_01_band = 0;
    this->svf_tilde_01_notch = 0;
    this->svf_tilde_01_dFreq_reset();
    this->svf_tilde_01_dQ_reset();
}

void svf_tilde_01_dspsetup(bool force) {
    if ((bool)(this->svf_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->svf_tilde_01_reset();
    this->svf_tilde_01_setupDone = true;
    this->svf_tilde_01_dFreq_dspsetup();
    this->svf_tilde_01_dQ_dspsetup();
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

void globaltransport_advance() {}

void globaltransport_dspsetup(bool ) {}

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
    limi_01_bypass = 0;
    limi_01_dcblock = 0;
    limi_01_lookahead = 100;
    limi_01_lookahead_setter(limi_01_lookahead);
    limi_01_preamp = 0;
    limi_01_preamp_setter(limi_01_preamp);
    limi_01_postamp = 0;
    limi_01_postamp_setter(limi_01_postamp);
    limi_01_threshold = 0;
    limi_01_threshold_setter(limi_01_threshold);
    limi_01_release = 1000;
    downsamp_tilde_01_x = 0;
    downsamp_tilde_01_downSample = 0;
    param_01_value = 0;
    expr_01_in1 = 0;
    expr_01_in2 = 1;
    expr_01_out1 = 0;
    selector_01_onoff = 1;
    svf_tilde_01_x = 0;
    svf_tilde_01_freq = 0;
    svf_tilde_01_q = 0;
    param_02_value = 20;
    param_03_value = 250;
    param_04_value = 0.5;
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
    limi_01_last = 0;
    limi_01_lookaheadIndex = 0;
    limi_01_recover = 0;
    limi_01_lookaheadInv = 0;
    limi_01_dc1_xm1 = 0;
    limi_01_dc1_ym1 = 0;
    limi_01_dc2_xm1 = 0;
    limi_01_dc2_ym1 = 0;
    limi_01_setupDone = false;
    downsamp_tilde_01_lastDownSample = 0;
    downsamp_tilde_01_current = 0;
    downsamp_tilde_01_hold = 0;
    param_01_lastValue = 0;
    svf_tilde_01_low = 0;
    svf_tilde_01_high = 0;
    svf_tilde_01_band = 0;
    svf_tilde_01_notch = 0;
    svf_tilde_01__freq = 0.001;
    svf_tilde_01__q = 0.001;
    svf_tilde_01_dFreq_prev = 0;
    svf_tilde_01_dQ_prev = 0;
    svf_tilde_01_setupDone = false;
    param_02_lastValue = 0;
    param_03_lastValue = 0;
    param_04_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_state = nullptr;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number limi_01_bypass;
    number limi_01_dcblock;
    number limi_01_lookahead;
    number limi_01_preamp;
    number limi_01_postamp;
    number limi_01_threshold;
    number limi_01_release;
    number downsamp_tilde_01_x;
    number downsamp_tilde_01_downSample;
    number param_01_value;
    number expr_01_in1;
    number expr_01_in2;
    number expr_01_out1;
    number selector_01_onoff;
    number svf_tilde_01_x;
    number svf_tilde_01_freq;
    number svf_tilde_01_q;
    number param_02_value;
    number param_03_value;
    number param_04_value;
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
    SampleValue limi_01_lookaheadBuffers[2][512] = { };
    SampleValue limi_01_gainBuffer[512] = { };
    number limi_01_last;
    int limi_01_lookaheadIndex;
    number limi_01_recover;
    number limi_01_lookaheadInv;
    number limi_01_dc1_xm1;
    number limi_01_dc1_ym1;
    number limi_01_dc2_xm1;
    number limi_01_dc2_ym1;
    bool limi_01_setupDone;
    number downsamp_tilde_01_lastDownSample;
    int downsamp_tilde_01_current;
    number downsamp_tilde_01_hold;
    number param_01_lastValue;
    number svf_tilde_01_low;
    number svf_tilde_01_high;
    number svf_tilde_01_band;
    number svf_tilde_01_notch;
    number svf_tilde_01__freq;
    number svf_tilde_01__q;
    number svf_tilde_01_dFreq_prev;
    number svf_tilde_01_dQ_prev;
    bool svf_tilde_01_setupDone;
    number param_02_lastValue;
    number param_03_lastValue;
    number param_04_lastValue;
    signal globaltransport_tempo;
    signal globaltransport_state;
    number stackprotect_count;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* createyoi()
{
    return new yoi();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr yoiFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return createyoi;
}

} // end RNBO namespace

