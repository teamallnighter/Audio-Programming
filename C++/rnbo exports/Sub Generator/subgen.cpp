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

class subgen : public PatcherInterfaceImpl {
public:

subgen()
{
}

~subgen()
{
}

subgen* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
    getEngine()->flushClockEvents(this, 1646922831, false);
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

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 1;
}

void processMidiEvent(MillisecondTime time, int port, ConstByteArray data, Index length) {
    this->updateTime(time);
    this->midiin_01_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
}

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
    this->ip_01_perform(this->signals[0], n);
    this->signalforwarder_01_perform(this->signals[0], out2, n);
    this->gen_01_perform(in1, this->gen_01_rampsamples, this->signals[1], this->dummyBuffer, n);
    this->snapshot_01_perform(this->signals[1], n);
    this->ip_02_perform(this->signals[1], n);
    this->mtof_tilde_01_perform(this->mtof_tilde_01_midivalue, this->signals[2], n);

    this->selector_01_perform(
        this->selector_01_onoff,
        this->signals[1],
        this->signals[2],
        this->signals[3],
        n
    );

    this->ip_03_perform(this->signals[2], n);
    this->dspexpr_02_perform(this->signals[3], this->signals[2], this->signals[1], n);

    this->cycle_tilde_01_perform(
        this->signals[1],
        this->cycle_tilde_01_phase_offset,
        this->signals[2],
        this->dummyBuffer,
        n
    );

    this->dspexpr_01_perform(this->signals[2], this->signals[0], this->signals[1], n);
    this->limi_01_perform(this->signals[1], this->signals[0], n);

    this->svf_tilde_01_perform(
        this->signals[0],
        this->svf_tilde_01_freq,
        this->svf_tilde_01_q,
        this->signals[1],
        this->dummyBuffer,
        this->dummyBuffer,
        this->dummyBuffer,
        n
    );

    this->limi_02_perform(this->signals[1], out1, n);
    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        Index i;

        for (i = 0; i < 4; i++) {
            this->signals[i] = resizeSignal(this->signals[i], this->maxvs, maxBlockSize);
        }

        this->ip_01_sigbuf = resizeSignal(this->ip_01_sigbuf, this->maxvs, maxBlockSize);
        this->ip_02_sigbuf = resizeSignal(this->ip_02_sigbuf, this->maxvs, maxBlockSize);
        this->ip_03_sigbuf = resizeSignal(this->ip_03_sigbuf, this->maxvs, maxBlockSize);
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

    this->ip_01_dspsetup(forceDSPSetup);
    this->gen_01_dspsetup(forceDSPSetup);
    this->ip_02_dspsetup(forceDSPSetup);
    this->ip_03_dspsetup(forceDSPSetup);
    this->cycle_tilde_01_dspsetup(forceDSPSetup);
    this->limi_01_dspsetup(forceDSPSetup);
    this->svf_tilde_01_dspsetup(forceDSPSetup);
    this->limi_02_dspsetup(forceDSPSetup);
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
    case 0:
        {
        return addressOf(this->RNBODefaultSinus);
        break;
        }
    case 1:
        {
        return addressOf(this->RNBODefaultMtofLookupTable256);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 2;
}

void fillRNBODefaultSinus(DataRef& ref) {
    Float64BufferRef buffer;
    buffer = new Float64Buffer(ref);
    number bufsize = buffer->getSize();

    for (Index i = 0; i < bufsize; i++) {
        buffer[i] = rnbo_cos(i * 3.14159265358979323846 * 2. / bufsize);
    }
}

void fillRNBODefaultMtofLookupTable256(DataRef& ref) {
    Float64BufferRef buffer;
    buffer = new Float64Buffer(ref);
    number bufsize = buffer->getSize();

    for (Index i = 0; i < bufsize; i++) {
        number midivalue = -256. + (number)512. / (bufsize - 1) * i;
        buffer[i] = rnbo_exp(.057762265 * (midivalue - 69.0));
    }
}

void fillDataRef(DataRefIndex index, DataRef& ref) {
    switch (index) {
    case 0:
        {
        this->fillRNBODefaultSinus(ref);
        break;
        }
    case 1:
        {
        this->fillRNBODefaultMtofLookupTable256(ref);
        break;
        }
    }
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->cycle_tilde_01_buffer = new Float64Buffer(this->RNBODefaultSinus);
        this->cycle_tilde_01_bufferUpdated();
    }

    if (index == 1) {
        this->mtof_tilde_01_innerMtoF_buffer = new Float64Buffer(this->RNBODefaultMtofLookupTable256);
    }
}

void initialize() {
    this->RNBODefaultSinus = initDataRef("RNBODefaultSinus", true, nullptr, "buffer~");
    this->RNBODefaultMtofLookupTable256 = initDataRef("RNBODefaultMtofLookupTable256", true, nullptr, "buffer~");
    this->assign_defaults();
    this->setState();
    this->RNBODefaultSinus->setIndex(0);
    this->cycle_tilde_01_buffer = new Float64Buffer(this->RNBODefaultSinus);
    this->RNBODefaultMtofLookupTable256->setIndex(1);
    this->mtof_tilde_01_innerMtoF_buffer = new Float64Buffer(this->RNBODefaultMtofLookupTable256);
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
    this->param_01_getPresetValue(getSubState(preset, "input_type"));
    this->param_02_getPresetValue(getSubState(preset, "rampsamples"));
    this->param_03_getPresetValue(getSubState(preset, "lowpass"));
    this->param_04_getPresetValue(getSubState(preset, "lowpass_q"));
    this->param_05_getPresetValue(getSubState(preset, "rise"));
    this->param_06_getPresetValue(getSubState(preset, "sub_octaves"));
    this->param_07_getPresetValue(getSubState(preset, "fall"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "input_type"));
    this->param_02_setPresetValue(getSubState(preset, "rampsamples"));
    this->param_03_setPresetValue(getSubState(preset, "lowpass"));
    this->param_04_setPresetValue(getSubState(preset, "lowpass_q"));
    this->param_05_setPresetValue(getSubState(preset, "rise"));
    this->param_06_setPresetValue(getSubState(preset, "sub_octaves"));
    this->param_07_setPresetValue(getSubState(preset, "fall"));
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
    case 4:
        {
        this->param_05_value_set(v);
        break;
        }
    case 5:
        {
        this->param_06_value_set(v);
        break;
        }
    case 6:
        {
        this->param_07_value_set(v);
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
    case 4:
        {
        return this->param_05_value;
        }
    case 5:
        {
        return this->param_06_value;
        }
    case 6:
        {
        return this->param_07_value;
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
    return 7;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "input_type";
        }
    case 1:
        {
        return "rampsamples";
        }
    case 2:
        {
        return "lowpass";
        }
    case 3:
        {
        return "lowpass_q";
        }
    case 4:
        {
        return "rise";
        }
    case 5:
        {
        return "sub_octaves";
        }
    case 6:
        {
        return "fall";
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
        return "input_type";
        }
    case 1:
        {
        return "rampsamples";
        }
    case 2:
        {
        return "lowpass";
        }
    case 3:
        {
        return "lowpass_q";
        }
    case 4:
        {
        return "rise";
        }
    case 5:
        {
        return "sub_octaves";
        }
    case 6:
        {
        return "fall";
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
            static const char * eVal0[] = {"freq_in", "midi_in"};
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
            info->initialValue = 0;
            info->min = 1;
            info->max = 48000;
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
            info->initialValue = 150;
            info->min = 20;
            info->max = 1000;
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
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 10000;
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
        case 5:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0;
            info->max = 2;
            info->exponent = 1;
            info->steps = 3;
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
        case 6:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 10000;
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
    case 5:
        {
        {
            value = (value < 0 ? 0 : (value > 2 ? 2 : value));
            ParameterValue normalizedValue = (value - 0) / (2 - 0);

            {
                normalizedValue = this->applyStepsToNormalizedParameterValue(normalizedValue, 3);
            }

            return normalizedValue;
        }
        }
    case 4:
    case 6:
        {
        {
            value = (value < 0 ? 0 : (value > 10000 ? 10000 : value));
            ParameterValue normalizedValue = (value - 0) / (10000 - 0);
            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < 1 ? 1 : (value > 48000 ? 48000 : value));
            ParameterValue normalizedValue = (value - 1) / (48000 - 1);
            return normalizedValue;
        }
        }
    case 2:
        {
        {
            value = (value < 20 ? 20 : (value > 1000 ? 1000 : value));
            ParameterValue normalizedValue = (value - 20) / (1000 - 20);
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
    case 5:
        {
        {
            {
                value = this->applyStepsToNormalizedParameterValue(value, 3);
            }

            {
                return 0 + value * (2 - 0);
            }
        }
        }
    case 4:
    case 6:
        {
        {
            {
                return 0 + value * (10000 - 0);
            }
        }
        }
    case 1:
        {
        {
            {
                return 1 + value * (48000 - 1);
            }
        }
        }
    case 2:
        {
        {
            {
                return 20 + value * (1000 - 20);
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
    case 4:
        {
        return this->param_05_value_constrain(value);
        }
    case 5:
        {
        return this->param_06_value_constrain(value);
        }
    case 6:
        {
        return this->param_07_value_constrain(value);
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
    case 1646922831:
        {
        this->snapshot_01_out_set(value);
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

void processNumMessage(MessageTag tag, MessageTag objectId, MillisecondTime time, number payload) {
    RNBO_UNUSED(objectId);
    this->updateTime(time);

    switch (tag) {
    case TAG("in2"):
        {
        this->eventinlet_01_out1_number_set(payload);
        break;
        }
    }
}

void processListMessage(
    MessageTag tag,
    MessageTag objectId,
    MillisecondTime time,
    const list& payload
) {
    RNBO_UNUSED(objectId);
    this->updateTime(time);

    switch (tag) {
    case TAG("in2"):
        {
        this->eventinlet_01_out1_list_set(payload);
        break;
        }
    }
}

void processBangMessage(MessageTag tag, MessageTag objectId, MillisecondTime time) {
    RNBO_UNUSED(objectId);
    this->updateTime(time);

    switch (tag) {
    case TAG("in2"):
        {
        this->eventinlet_01_out1_bang_bang();
        break;
        }
    }
}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {
    case TAG("in2"):
        {
        return "in2";
        }
    case TAG(""):
        {
        return "";
        }
    }

    return "";
}

MessageIndex getNumMessages() const {
    return 1;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {
    case 0:
        {
        static const MessageInfo r0 = {
            "in2",
            Inport
        };

        return r0;
        }
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

    this->intnum_01_input_number_set(v);
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

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        this->getEngine()->presetTouched();
        this->param_05_lastValue = this->param_05_value;
    }

    this->rampsmooth_01_up_set(v);
}

void param_06_value_set(number v) {
    v = this->param_06_value_constrain(v);
    this->param_06_value = v;
    this->sendParameter(5, false);

    if (this->param_06_value != this->param_06_lastValue) {
        this->getEngine()->presetTouched();
        this->param_06_lastValue = this->param_06_value;
    }

    this->expr_02_in1_set(v);
}

void param_07_value_set(number v) {
    v = this->param_07_value_constrain(v);
    this->param_07_value = v;
    this->sendParameter(6, false);

    if (this->param_07_value != this->param_07_lastValue) {
        this->getEngine()->presetTouched();
        this->param_07_lastValue = this->param_07_value;
    }

    this->rampsmooth_01_down_set(v);
}

void snapshot_01_out_set(number v) {
    this->snapshot_01_out = v;
    this->rampsmooth_01_x_set(v);
}

void eventinlet_01_out1_bang_bang() {
    this->ip_02_value_bang();
}

void eventinlet_01_out1_number_set(number v) {
    this->ip_02_value_set(v);
}

void eventinlet_01_out1_list_set(const list& v) {
    {
        if (v->length > 1)
            this->ip_02_impulse_set(v[1]);

        number converted = (v->length > 0 ? v[0] : 0);
        this->ip_02_value_set(converted);
    }
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

void allocateDataRefs() {
    this->cycle_tilde_01_buffer->requestSize(16384, 1);
    this->cycle_tilde_01_buffer->setSampleRate(this->sr);
    this->mtof_tilde_01_innerMtoF_buffer->requestSize(65536, 1);
    this->mtof_tilde_01_innerMtoF_buffer->setSampleRate(this->sr);
    this->cycle_tilde_01_buffer = this->cycle_tilde_01_buffer->allocateIfNeeded();

    if (this->RNBODefaultSinus->hasRequestedSize()) {
        if (this->RNBODefaultSinus->wantsFill())
            this->fillRNBODefaultSinus(this->RNBODefaultSinus);

        this->getEngine()->sendDataRefUpdated(0);
    }

    this->mtof_tilde_01_innerMtoF_buffer = this->mtof_tilde_01_innerMtoF_buffer->allocateIfNeeded();

    if (this->RNBODefaultMtofLookupTable256->hasRequestedSize()) {
        if (this->RNBODefaultMtofLookupTable256->wantsFill())
            this->fillRNBODefaultMtofLookupTable256(this->RNBODefaultMtofLookupTable256);

        this->getEngine()->sendDataRefUpdated(1);
    }
}

void initializeObjects() {
    this->ip_01_init();
    this->gen_01_history_1_init();
    this->gen_01_history_2_init();
    this->gen_01_history_3_init();
    this->gen_01_counter_10_init();
    this->ip_02_init();
    this->mtof_tilde_01_innerScala_init();
    this->mtof_tilde_01_init();
    this->ip_03_init();
}

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

    {
        this->scheduleParamInit(4, 0);
    }

    {
        this->scheduleParamInit(5, 0);
    }

    {
        this->scheduleParamInit(6, 0);
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
    this->expr_01_out1_set(this->expr_01_in1 + this->expr_01_in2);//#map:+_obj-55:1
}

number param_02_value_constrain(number v) const {
    v = (v > 48000 ? 48000 : (v < 1 ? 1 : v));
    return v;
}

number gen_01_rampsamples_constrain(number v) const {
    if (v < 1)
        v = 1;

    return v;
}

void gen_01_rampsamples_set(number v) {
    v = this->gen_01_rampsamples_constrain(v);
    this->gen_01_rampsamples = v;
}

void intnum_01_out_set(number v) {
    this->gen_01_rampsamples_set(v);
}

void intnum_01_input_number_set(number v) {
    this->intnum_01_input_number = v;
    this->intnum_01_stored = v;
    this->intnum_01_out_set(trunc(v));
}

number param_03_value_constrain(number v) const {
    v = (v > 1000 ? 1000 : (v < 20 ? 20 : v));
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

number param_05_value_constrain(number v) const {
    v = (v > 10000 ? 10000 : (v < 0 ? 0 : v));
    return v;
}

void rampsmooth_01_up_set(number v) {
    this->rampsmooth_01_up = v;
}

number param_06_value_constrain(number v) const {
    v = (v > 2 ? 2 : (v < 0 ? 0 : v));

    {
        number oneStep = (number)2 / (number)2;
        number oneStepInv = (oneStep != 0 ? (number)1 / oneStep : 0);
        number numberOfSteps = rnbo_fround(v * oneStepInv * 1 / (number)1) * 1;
        v = numberOfSteps * oneStep;
    }

    return v;
}

void ip_03_value_set(number v) {
    this->ip_03_value = v;
    this->ip_03_fillSigBuf();
    this->ip_03_lastValue = v;
}

void expr_02_out1_set(number v) {
    this->expr_02_out1 = v;
    this->ip_03_value_set(this->expr_02_out1);
}

void expr_02_in1_set(number in1) {
    this->expr_02_in1 = in1;
    this->expr_02_out1_set(this->expr_02_in1 * this->expr_02_in2);//#map:*_obj-9:1
}

number param_07_value_constrain(number v) const {
    v = (v > 10000 ? 10000 : (v < 0 ? 0 : v));
    return v;
}

void rampsmooth_01_down_set(number v) {
    this->rampsmooth_01_down = v;
}

void ip_01_value_set(number v) {
    this->ip_01_value = v;
    this->ip_01_fillSigBuf();
    this->ip_01_lastValue = v;
}

void rampsmooth_01_out1_set(number v) {
    this->ip_01_value_set(v);
}

void rampsmooth_01_x_set(number x) {
    this->rampsmooth_01_x = x;
    auto down = this->rampsmooth_01_down;
    auto up = this->rampsmooth_01_up;

    if (this->rampsmooth_01_d_next(x) != 0.) {
        if (x > this->rampsmooth_01_prev) {
            number _up = up;

            if (_up < 1)
                _up = 1;

            this->rampsmooth_01_index = _up;
            this->rampsmooth_01_increment = (x - this->rampsmooth_01_prev) / _up;
        } else if (x < this->rampsmooth_01_prev) {
            number _down = down;

            if (_down < 1)
                _down = 1;

            this->rampsmooth_01_index = _down;
            this->rampsmooth_01_increment = (x - this->rampsmooth_01_prev) / _down;
        }
    }

    if (this->rampsmooth_01_index > 0) {
        this->rampsmooth_01_prev += this->rampsmooth_01_increment;
        this->rampsmooth_01_index -= 1;
    } else {
        this->rampsmooth_01_prev = x;
    }

    {
        this->rampsmooth_01_out1_set(this->rampsmooth_01_prev);
        return;
    }
}

void ip_02_value_bang() {
    number v = this->ip_02_value;
    this->ip_02_fillSigBuf();
    this->ip_02_lastValue = v;
}

void ip_02_value_set(number v) {
    this->ip_02_value = v;
    this->ip_02_fillSigBuf();
    this->ip_02_lastValue = v;
}

void ip_02_impulse_set(number v) {
    this->ip_02_impulse = v;
    this->ip_02_fillSigBuf();
    this->ip_02_lastValue = 0;
    SampleIndex lengthInSamples = (SampleIndex)(this->maximum(1, this->msToSamps(v, this->sr)));
    this->ip_02_resetCount = lengthInSamples;
}

void midiparse_01_channel_set(number ) {}

void unpack_01_out2_set(number v) {
    this->unpack_01_out2 = v;
}

void mtof_tilde_01_midivalue_set(number v) {
    this->mtof_tilde_01_midivalue = v;
}

void unpack_01_out1_set(number v) {
    this->unpack_01_out1 = v;
    this->mtof_tilde_01_midivalue_set(v);
}

void unpack_01_input_list_set(const list& v) {
    if (v->length > 1)
        this->unpack_01_out2_set(v[1]);

    if (v->length > 0)
        this->unpack_01_out1_set(v[0]);
}

void midiparse_01_noteonoroff_set(const list& v) {
    this->unpack_01_input_list_set(v);
}

void midiparse_01_polypressure_set(const list& ) {}

void midiparse_01_controlchange_set(const list& ) {}

void midiparse_01_programchange_set(number ) {}

void midiparse_01_aftertouch_set(number ) {}

void midiparse_01_pitchbend_set(number ) {}

void midiparse_01_midiin_set(number midivalue) {
    list result = this->midiparse_01_parser_next(midivalue);

    switch ((int)result[0]) {
    case 0:
        {
        this->midiparse_01_channel_set(result[3]);
        this->midiparse_01_noteonoroff_set({result[1], result[2]});
        break;
        }
    case 1:
        {
        this->midiparse_01_channel_set(result[3]);
        this->midiparse_01_polypressure_set({result[1], result[2]});
        break;
        }
    case 2:
        {
        this->midiparse_01_channel_set(result[3]);
        this->midiparse_01_controlchange_set({result[1], result[2]});
        break;
        }
    case 3:
        {
        this->midiparse_01_channel_set(result[2]);
        this->midiparse_01_programchange_set(result[1]);
        break;
        }
    case 4:
        {
        this->midiparse_01_channel_set(result[2]);
        this->midiparse_01_aftertouch_set(result[1]);
        break;
        }
    case 5:
        {
        this->midiparse_01_channel_set(result[2]);
        this->midiparse_01_pitchbend_set(result[1]);
        break;
        }
    }
}

void midiin_01_midiout_set(number v) {
    this->midiparse_01_midiin_set(v);
}

void midiin_01_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(port);
    RNBO_UNUSED(channel);
    RNBO_UNUSED(status);
    Index i;

    for (i = 0; i < length; i++) {
        this->midiin_01_midiout_set(data[i]);
    }
}

void ip_01_perform(SampleValue * out, Index n) {
    auto __ip_01_lastValue = this->ip_01_lastValue;
    auto __ip_01_lastIndex = this->ip_01_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_01_lastIndex ? __ip_01_lastValue : this->ip_01_sigbuf[(Index)i]);
    }

    __ip_01_lastIndex = 0;
    this->ip_01_lastIndex = __ip_01_lastIndex;
}

void signalforwarder_01_perform(const SampleValue * input, SampleValue * output, Index n) {
    for (Index i = 0; i < n; i++) {
        output[(Index)i] = input[(Index)i];
    }
}

void gen_01_perform(
    const Sample * in1,
    number rampsamples,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    auto __gen_01_history_3_value = this->gen_01_history_3_value;
    auto __gen_01_history_1_value = this->gen_01_history_1_value;
    auto __gen_01_history_2_value = this->gen_01_history_2_value;
    number expr_4_0 = rampsamples;
    Index i;

    for (i = 0; i < n; i++) {
        number gte_5_1 = __gen_01_history_2_value >= rampsamples;
        number abs_6_2 = rnbo_abs(in1[(Index)i]);
        number gt_7_3 = abs_6_2 > __gen_01_history_1_value;
        number or_8_4 = (bool)(gt_7_3) || (bool)(gte_5_1);
        number latch_9_6 = this->gen_01_latch_5_next(abs_6_2, or_8_4);
        number latch_10_8 = this->gen_01_latch_7_next(__gen_01_history_3_value, or_8_4);
        number sub_11_9 = latch_9_6 - latch_10_8;
        number counter_12 = 0;
        number counter_13 = 0;
        number counter_14 = 0;
        array<number, 3> result_11 = this->gen_01_counter_10_next(1, or_8_4 + gte_5_1, 0);
        counter_14 = result_11[2];
        counter_13 = result_11[1];
        counter_12 = result_11[0];
        number add_15_12 = counter_12 + 1;
        number div_16_13 = (rampsamples == 0. ? 0. : add_15_12 / rampsamples);
        number mul_17_14 = sub_11_9 * div_16_13;
        number add_18_15 = latch_10_8 + mul_17_14;
        number expr_19_16 = add_18_15;
        number history_1_next_20_17 = fixdenorm(latch_9_6);
        number history_2_next_21_18 = fixdenorm(add_15_12);
        number history_3_next_22_19 = fixdenorm(add_18_15);
        __gen_01_history_1_value = history_1_next_20_17;
        __gen_01_history_3_value = history_3_next_22_19;
        __gen_01_history_2_value = history_2_next_21_18;
        out2[(Index)i] = expr_4_0;
        out1[(Index)i] = expr_19_16;
    }

    this->gen_01_history_2_value = __gen_01_history_2_value;
    this->gen_01_history_1_value = __gen_01_history_1_value;
    this->gen_01_history_3_value = __gen_01_history_3_value;
}

void snapshot_01_perform(const SampleValue * input_signal, Index n) {
    auto __snapshot_01_lastValue = this->snapshot_01_lastValue;
    auto __snapshot_01_calc = this->snapshot_01_calc;
    auto __snapshot_01_count = this->snapshot_01_count;
    auto __snapshot_01_nextTime = this->snapshot_01_nextTime;
    auto __snapshot_01_interval = this->snapshot_01_interval;
    number timeInSamples = this->msToSamps(__snapshot_01_interval, this->sr);

    if (__snapshot_01_interval > 0) {
        for (Index i = 0; i < n; i++) {
            if (__snapshot_01_nextTime <= __snapshot_01_count + (SampleIndex)(i)) {
                {
                    __snapshot_01_calc = input_signal[(Index)i];
                }

                this->getEngine()->scheduleClockEventWithValue(
                    this,
                    1646922831,
                    this->sampsToMs((SampleIndex)(this->vs)) + this->_currentTime,
                    __snapshot_01_calc
                );;

                __snapshot_01_calc = 0;
                __snapshot_01_nextTime += timeInSamples;
            }
        }

        __snapshot_01_count += this->vs;
    }

    __snapshot_01_lastValue = input_signal[(Index)(n - 1)];
    this->snapshot_01_nextTime = __snapshot_01_nextTime;
    this->snapshot_01_count = __snapshot_01_count;
    this->snapshot_01_calc = __snapshot_01_calc;
    this->snapshot_01_lastValue = __snapshot_01_lastValue;
}

void ip_02_perform(SampleValue * out, Index n) {
    auto __ip_02_lastValue = this->ip_02_lastValue;
    auto __ip_02_lastIndex = this->ip_02_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_02_lastIndex ? __ip_02_lastValue : this->ip_02_sigbuf[(Index)i]);
    }

    __ip_02_lastIndex = 0;
    this->ip_02_lastIndex = __ip_02_lastIndex;
}

void mtof_tilde_01_perform(number midivalue, SampleValue * out, Index n) {
    auto __mtof_tilde_01_base = this->mtof_tilde_01_base;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = this->mtof_tilde_01_innerMtoF_next(midivalue, __mtof_tilde_01_base);
    }
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

void ip_03_perform(SampleValue * out, Index n) {
    auto __ip_03_lastValue = this->ip_03_lastValue;
    auto __ip_03_lastIndex = this->ip_03_lastIndex;

    for (Index i = 0; i < n; i++) {
        out[(Index)i] = ((SampleIndex)(i) >= __ip_03_lastIndex ? __ip_03_lastValue : this->ip_03_sigbuf[(Index)i]);
    }

    __ip_03_lastIndex = 0;
    this->ip_03_lastIndex = __ip_03_lastIndex;
}

void dspexpr_02_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] + in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void cycle_tilde_01_perform(
    const Sample * frequency,
    number phase_offset,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    RNBO_UNUSED(phase_offset);
    auto __cycle_tilde_01_f2i = this->cycle_tilde_01_f2i;
    auto __cycle_tilde_01_buffer = this->cycle_tilde_01_buffer;
    auto __cycle_tilde_01_phasei = this->cycle_tilde_01_phasei;
    Index i;

    for (i = 0; i < n; i++) {
        {
            uint32_t uint_phase;

            {
                {
                    uint_phase = __cycle_tilde_01_phasei;
                }
            }

            uint32_t idx = (uint32_t)(uint32_rshift(uint_phase, 18));
            number frac = ((BinOpInt)((BinOpInt)uint_phase & (BinOpInt)262143)) * 3.81471181759574e-6;
            number y0 = __cycle_tilde_01_buffer[(Index)idx];
            number y1 = __cycle_tilde_01_buffer[(Index)((BinOpInt)(idx + 1) & (BinOpInt)16383)];
            number y = y0 + frac * (y1 - y0);

            {
                uint32_t pincr = (uint32_t)(uint32_trunc(frequency[(Index)i] * __cycle_tilde_01_f2i));
                __cycle_tilde_01_phasei = uint32_add(__cycle_tilde_01_phasei, pincr);
            }

            out1[(Index)i] = y;
            out2[(Index)i] = uint_phase * 0.232830643653869629e-9;
            continue;
        }
    }

    this->cycle_tilde_01_phasei = __cycle_tilde_01_phasei;
}

void dspexpr_01_perform(const Sample * in1, const Sample * in2, SampleValue * out1, Index n) {
    Index i;

    for (i = 0; i < n; i++) {
        out1[(Index)i] = in1[(Index)i] * in2[(Index)i];//#map:_###_obj_###_:1
    }
}

void limi_01_perform(const SampleValue * input1, SampleValue * output1, Index n) {
    RNBO_UNUSED(output1);
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
    ConstSampleArray<1> input = {input1};
    SampleArray<1> output = {output1};

    if ((bool)(__limi_01_bypass)) {
        for (Index i = 0; i < n; i++) {
            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = input[(Index)j][(Index)i];
            }
        }
    } else {
        number v;

        for (Index i = 0; i < n; i++) {
            number hotSample = 0;

            for (Index j = 0; j < 1; j++) {
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

            for (Index j = 0; j < 1; j++) {
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

void limi_02_perform(const SampleValue * input1, SampleValue * output1, Index n) {
    RNBO_UNUSED(output1);
    RNBO_UNUSED(input1);
    auto __limi_02_lookaheadInv = this->limi_02_lookaheadInv;
    auto __limi_02_threshold = this->limi_02_threshold;
    auto __limi_02_lookahead = this->limi_02_lookahead;
    auto __limi_02_recover = this->limi_02_recover;
    auto __limi_02_last = this->limi_02_last;
    auto __limi_02_postamp = this->limi_02_postamp;
    auto __limi_02_lookaheadIndex = this->limi_02_lookaheadIndex;
    auto __limi_02_preamp = this->limi_02_preamp;
    auto __limi_02_dcblock = this->limi_02_dcblock;
    auto __limi_02_bypass = this->limi_02_bypass;
    ConstSampleArray<1> input = {input1};
    SampleArray<1> output = {output1};

    if ((bool)(__limi_02_bypass)) {
        for (Index i = 0; i < n; i++) {
            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = input[(Index)j][(Index)i];
            }
        }
    } else {
        number v;

        for (Index i = 0; i < n; i++) {
            number hotSample = 0;

            for (Index j = 0; j < 1; j++) {
                auto smps = input[(Index)j];
                v = ((bool)(__limi_02_dcblock) ? this->limi_02_dc_next(j, smps[(Index)i], 0.9997) : smps[(Index)i]);
                v *= __limi_02_preamp;
                this->limi_02_lookaheadBuffers[(Index)j][__limi_02_lookaheadIndex] = v * __limi_02_postamp;
                v = rnbo_fabs(v);

                if (v > hotSample)
                    hotSample = v;
            }

            {
                if (__limi_02_last > 0.01)
                    v = __limi_02_last + __limi_02_recover * __limi_02_last;
                else
                    v = __limi_02_last + __limi_02_recover;
            }

            if (v > 1)
                v = 1;

            this->limi_02_gainBuffer[__limi_02_lookaheadIndex] = v;
            int lookaheadPlayback = (int)(__limi_02_lookaheadIndex - (int)(__limi_02_lookahead));

            if (lookaheadPlayback < 0)
                lookaheadPlayback += (int)(__limi_02_lookahead);

            if (hotSample * v > __limi_02_threshold) {
                number newgain;
                number curgain = __limi_02_threshold / hotSample;
                number inc = __limi_02_threshold - curgain;
                number acc = 0.0;
                number flag = 0;

                for (Index j = 0; flag == 0 && j < (Index)(__limi_02_lookahead); j++) {
                    int k = (int)(__limi_02_lookaheadIndex - (int)(j));

                    if (k < 0)
                        k += (int)(__limi_02_lookahead);

                    {
                        newgain = curgain + inc * (acc * acc);
                    }

                    if (newgain < this->limi_02_gainBuffer[(Index)k])
                        this->limi_02_gainBuffer[(Index)k] = newgain;
                    else
                        flag = 1;

                    acc = acc + __limi_02_lookaheadInv;
                }
            }

            for (Index j = 0; j < 1; j++) {
                output[(Index)j][(Index)i] = this->limi_02_lookaheadBuffers[(Index)j][(Index)lookaheadPlayback] * this->limi_02_gainBuffer[(Index)lookaheadPlayback];
            }

            __limi_02_last = this->limi_02_gainBuffer[__limi_02_lookaheadIndex];
            __limi_02_lookaheadIndex++;

            if (__limi_02_lookaheadIndex >= __limi_02_lookahead)
                __limi_02_lookaheadIndex = 0;
        }
    }

    this->limi_02_lookaheadIndex = __limi_02_lookaheadIndex;
    this->limi_02_last = __limi_02_last;
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

void limi_02_lookahead_setter(number v) {
    this->limi_02_lookahead = (v > 512 ? 512 : (v < 0 ? 0 : v));
    this->limi_02_lookaheadInv = (number)1 / this->limi_02_lookahead;
}

void limi_02_preamp_setter(number v) {
    this->limi_02_preamp = rnbo_pow(10., v * 0.05);
}

void limi_02_postamp_setter(number v) {
    this->limi_02_postamp = rnbo_pow(10., v * 0.05);
}

void limi_02_threshold_setter(number v) {
    this->limi_02_threshold = rnbo_pow(10., v * 0.05);
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

number cycle_tilde_01_ph_next(number freq, number reset) {
    {
        {
            if (reset >= 0.)
                this->cycle_tilde_01_ph_currentPhase = reset;
        }
    }

    number pincr = freq * this->cycle_tilde_01_ph_conv;

    if (this->cycle_tilde_01_ph_currentPhase < 0.)
        this->cycle_tilde_01_ph_currentPhase = 1. + this->cycle_tilde_01_ph_currentPhase;

    if (this->cycle_tilde_01_ph_currentPhase > 1.)
        this->cycle_tilde_01_ph_currentPhase = this->cycle_tilde_01_ph_currentPhase - 1.;

    number tmp = this->cycle_tilde_01_ph_currentPhase;
    this->cycle_tilde_01_ph_currentPhase += pincr;
    return tmp;
}

void cycle_tilde_01_ph_reset() {
    this->cycle_tilde_01_ph_currentPhase = 0;
}

void cycle_tilde_01_ph_dspsetup() {
    this->cycle_tilde_01_ph_conv = (number)1 / this->sr;
}

void cycle_tilde_01_dspsetup(bool force) {
    if ((bool)(this->cycle_tilde_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->cycle_tilde_01_phasei = 0;
    this->cycle_tilde_01_f2i = (number)4294967296 / this->samplerate();
    this->cycle_tilde_01_wrap = (long)(this->cycle_tilde_01_buffer->getSize()) - 1;
    this->cycle_tilde_01_setupDone = true;
    this->cycle_tilde_01_ph_dspsetup();
}

void cycle_tilde_01_bufferUpdated() {
    this->cycle_tilde_01_wrap = (long)(this->cycle_tilde_01_buffer->getSize()) - 1;
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

number limi_01_dc_next(Index i, number x, number gain) {
    switch ((int)i) {
    default:
        {
        return this->limi_01_dc1_next(x, gain);
        }
    }

    return 0;
}

void limi_01_dc_reset(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_01_dc1_reset();
        }
    }
}

void limi_01_dc_dspsetup(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_01_dc1_dspsetup();
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
}

number limi_02_dc1_next(number x, number gain) {
    number y = x - this->limi_02_dc1_xm1 + this->limi_02_dc1_ym1 * gain;
    this->limi_02_dc1_xm1 = x;
    this->limi_02_dc1_ym1 = y;
    return y;
}

void limi_02_dc1_reset() {
    this->limi_02_dc1_xm1 = 0;
    this->limi_02_dc1_ym1 = 0;
}

void limi_02_dc1_dspsetup() {
    this->limi_02_dc1_reset();
}

number limi_02_dc_next(Index i, number x, number gain) {
    switch ((int)i) {
    default:
        {
        return this->limi_02_dc1_next(x, gain);
        }
    }

    return 0;
}

void limi_02_dc_reset(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_02_dc1_reset();
        }
    }
}

void limi_02_dc_dspsetup(Index i) {
    switch ((int)i) {
    default:
        {
        return this->limi_02_dc1_dspsetup();
        }
    }
}

void limi_02_reset() {
    this->limi_02_recover = (number)1000 / (this->limi_02_release * this->samplerate());

    {
        this->limi_02_recover *= 0.707;
    }
}

void limi_02_dspsetup(bool force) {
    if ((bool)(this->limi_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->limi_02_reset();
    this->limi_02_setupDone = true;
    this->limi_02_dc1_dspsetup();
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

void ip_01_init() {
    this->ip_01_lastValue = this->ip_01_value;
}

void ip_01_fillSigBuf() {
    if ((bool)(this->ip_01_sigbuf)) {
        SampleIndex k = (SampleIndex)(this->sampleOffsetIntoNextAudioBuffer);

        if (k >= (SampleIndex)(this->vs))
            k = (SampleIndex)(this->vs) - 1;

        for (SampleIndex i = (SampleIndex)(this->ip_01_lastIndex); i < k; i++) {
            if (this->ip_01_resetCount > 0) {
                this->ip_01_sigbuf[(Index)i] = 1;
                this->ip_01_resetCount--;
            } else {
                this->ip_01_sigbuf[(Index)i] = this->ip_01_lastValue;
            }
        }

        this->ip_01_lastIndex = k;
    }
}

void ip_01_dspsetup(bool force) {
    if ((bool)(this->ip_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_01_lastIndex = 0;
    this->ip_01_setupDone = true;
}

number rampsmooth_01_d_next(number x) {
    number temp = (number)(x - this->rampsmooth_01_d_prev);
    this->rampsmooth_01_d_prev = x;
    return temp;
}

void rampsmooth_01_d_dspsetup() {
    this->rampsmooth_01_d_reset();
}

void rampsmooth_01_d_reset() {
    this->rampsmooth_01_d_prev = 0;
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

number gen_01_history_1_getvalue() {
    return this->gen_01_history_1_value;
}

void gen_01_history_1_setvalue(number val) {
    this->gen_01_history_1_value = val;
}

void gen_01_history_1_reset() {
    this->gen_01_history_1_value = 0;
}

void gen_01_history_1_init() {
    this->gen_01_history_1_value = 0;
}

number gen_01_history_2_getvalue() {
    return this->gen_01_history_2_value;
}

void gen_01_history_2_setvalue(number val) {
    this->gen_01_history_2_value = val;
}

void gen_01_history_2_reset() {
    this->gen_01_history_2_value = 0;
}

void gen_01_history_2_init() {
    this->gen_01_history_2_value = 0;
}

number gen_01_history_3_getvalue() {
    return this->gen_01_history_3_value;
}

void gen_01_history_3_setvalue(number val) {
    this->gen_01_history_3_value = val;
}

void gen_01_history_3_reset() {
    this->gen_01_history_3_value = 0;
}

void gen_01_history_3_init() {
    this->gen_01_history_3_value = 0;
}

number gen_01_latch_5_next(number x, number control) {
    if (control != 0.)
        this->gen_01_latch_5_value = x;

    return this->gen_01_latch_5_value;
}

void gen_01_latch_5_dspsetup() {
    this->gen_01_latch_5_reset();
}

void gen_01_latch_5_reset() {
    this->gen_01_latch_5_value = 0;
}

number gen_01_latch_7_next(number x, number control) {
    if (control != 0.)
        this->gen_01_latch_7_value = x;

    return this->gen_01_latch_7_value;
}

void gen_01_latch_7_dspsetup() {
    this->gen_01_latch_7_reset();
}

void gen_01_latch_7_reset() {
    this->gen_01_latch_7_value = 0;
}

array<number, 3> gen_01_counter_10_next(number a, number reset, number limit) {
    RNBO_UNUSED(limit);
    RNBO_UNUSED(a);
    number carry_flag = 0;

    if (reset != 0) {
        this->gen_01_counter_10_count = 0;
        this->gen_01_counter_10_carry = 0;
    } else {
        this->gen_01_counter_10_count += 1;
    }

    return {this->gen_01_counter_10_count, carry_flag, this->gen_01_counter_10_carry};
}

void gen_01_counter_10_init() {
    this->gen_01_counter_10_count = -1;
}

void gen_01_counter_10_reset() {
    this->gen_01_counter_10_carry = 0;
    this->gen_01_counter_10_count = 0;
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_latch_5_dspsetup();
    this->gen_01_latch_7_dspsetup();
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void ip_02_init() {
    this->ip_02_lastValue = this->ip_02_value;
}

void ip_02_fillSigBuf() {
    if ((bool)(this->ip_02_sigbuf)) {
        SampleIndex k = (SampleIndex)(this->sampleOffsetIntoNextAudioBuffer);

        if (k >= (SampleIndex)(this->vs))
            k = (SampleIndex)(this->vs) - 1;

        for (SampleIndex i = (SampleIndex)(this->ip_02_lastIndex); i < k; i++) {
            if (this->ip_02_resetCount > 0) {
                this->ip_02_sigbuf[(Index)i] = 1;
                this->ip_02_resetCount--;
            } else {
                this->ip_02_sigbuf[(Index)i] = this->ip_02_lastValue;
            }
        }

        this->ip_02_lastIndex = k;
    }
}

void ip_02_dspsetup(bool force) {
    if ((bool)(this->ip_02_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_02_lastIndex = 0;
    this->ip_02_setupDone = true;
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

list midiparse_01_parser_next(number midiin) {
    int midivalue = (int)(trunc(midiin));
    list result = list(-1);
    number resetByte1 = false;

    if (midivalue > 127) {
        this->midiparse_01_parser_status = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0xFF);
        this->midiparse_01_parser_byte1 = -1;
    } else {
        switch ((int)((BinOpInt)this->midiparse_01_parser_status & (BinOpInt)0xF0)) {
        case 0xB0:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    2,
                    this->midiparse_01_parser_byte1,
                    (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xA0:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    1,
                    this->midiparse_01_parser_byte1,
                    (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xE0:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                number pitchbend;

                {
                    number val = this->midiparse_01_parser_byte1 + ((BinOpInt)(((BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F)) << imod_nocast((UBinOpInt)7, 32)));

                    {
                        val -= 0x2000;
                        pitchbend = val / (number)8192;
                    }
                }

                result = {
                    5,
                    pitchbend,
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xD0:
            {
            result = {
                4,
                (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
            };

            break;
            }
        case 0x90:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    0,
                    this->midiparse_01_parser_byte1,
                    (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        case 0xC0:
            {
            result = {
                3,
                (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F),
                (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
            };

            break;
            }
        case 0x80:
            {
            if (this->midiparse_01_parser_byte1 == -1) {
                this->midiparse_01_parser_byte1 = (BinOpInt)((BinOpInt)midivalue & (BinOpInt)0x7F);
            } else {
                result = {
                    0,
                    this->midiparse_01_parser_byte1,
                    0,
                    (BinOpInt)((BinOpInt)(1 + this->midiparse_01_parser_status) & (BinOpInt)0x0F)
                };

                resetByte1 = true;
            }

            break;
            }
        default:
            {
            result = {-1};
            }
        }
    }

    if (result->length > 1) {
        result->push(this->midiparse_01_parser_status);
        result->push(this->midiparse_01_parser_byte1);
        result->push(midivalue);
    }

    if ((bool)(resetByte1)) {
        this->midiparse_01_parser_byte1 = -1;
    }

    return result;
}

void midiparse_01_parser_reset() {
    this->midiparse_01_parser_status = -1;
    this->midiparse_01_parser_byte1 = -1;
}

number mtof_tilde_01_innerMtoF_next(number midivalue, number tuning) {
    if (midivalue == this->mtof_tilde_01_innerMtoF_lastInValue && tuning == this->mtof_tilde_01_innerMtoF_lastTuning)
        return this->mtof_tilde_01_innerMtoF_lastOutValue;

    this->mtof_tilde_01_innerMtoF_lastInValue = midivalue;
    this->mtof_tilde_01_innerMtoF_lastTuning = tuning;
    number result = 0;

    {
        result = rnbo_exp(.057762265 * (midivalue - 69.0));
    }

    this->mtof_tilde_01_innerMtoF_lastOutValue = tuning * result;
    return this->mtof_tilde_01_innerMtoF_lastOutValue;
}

void mtof_tilde_01_innerMtoF_reset() {
    this->mtof_tilde_01_innerMtoF_lastInValue = 0;
    this->mtof_tilde_01_innerMtoF_lastOutValue = 0;
    this->mtof_tilde_01_innerMtoF_lastTuning = 0;
}

void mtof_tilde_01_innerScala_mid(int v) {
    this->mtof_tilde_01_innerScala_kbmMid = v;
    this->mtof_tilde_01_innerScala_updateRefFreq();
}

void mtof_tilde_01_innerScala_ref(int v) {
    this->mtof_tilde_01_innerScala_kbmRefNum = v;
    this->mtof_tilde_01_innerScala_updateRefFreq();
}

void mtof_tilde_01_innerScala_base(number v) {
    this->mtof_tilde_01_innerScala_kbmRefFreq = v;
    this->mtof_tilde_01_innerScala_updateRefFreq();
}

void mtof_tilde_01_innerScala_init() {
    list sclValid = {
        12,
        100,
        0,
        200,
        0,
        300,
        0,
        400,
        0,
        500,
        0,
        600,
        0,
        700,
        0,
        800,
        0,
        900,
        0,
        1000,
        0,
        1100,
        0,
        2,
        1
    };

    this->mtof_tilde_01_innerScala_updateScale(sclValid);
}

void mtof_tilde_01_innerScala_update(list scale, list map) {
    if (scale->length > 0) {
        this->mtof_tilde_01_innerScala_updateScale(scale);
    }

    if (map->length > 0) {
        this->mtof_tilde_01_innerScala_updateMap(map);
    }
}

number mtof_tilde_01_innerScala_mtof(number note) {
    if ((bool)(this->mtof_tilde_01_innerScala_lastValid) && this->mtof_tilde_01_innerScala_lastNote == note) {
        return this->mtof_tilde_01_innerScala_lastFreq;
    }

    array<int, 2> degoct = this->mtof_tilde_01_innerScala_applyKBM(note);
    number out = 0;

    if (degoct[1] > 0) {
        out = this->mtof_tilde_01_innerScala_applySCL(degoct[0], fract(note), this->mtof_tilde_01_innerScala_refFreq);
    }

    this->mtof_tilde_01_innerScala_updateLast(note, out);
    return out;
}

number mtof_tilde_01_innerScala_ftom(number hz) {
    if (hz <= 0.0) {
        return 0.0;
    }

    if ((bool)(this->mtof_tilde_01_innerScala_lastValid) && this->mtof_tilde_01_innerScala_lastFreq == hz) {
        return this->mtof_tilde_01_innerScala_lastNote;
    }

    array<number, 2> df = this->mtof_tilde_01_innerScala_hztodeg(hz);
    int degree = (int)(df[0]);
    number frac = df[1];
    number out = 0;

    if (this->mtof_tilde_01_innerScala_kbmSize == 0) {
        out = this->mtof_tilde_01_innerScala_kbmMid + degree;
    } else {
        array<int, 2> octdeg = this->mtof_tilde_01_innerScala_octdegree(degree, this->mtof_tilde_01_innerScala_kbmOctaveDegree);
        number oct = (number)(octdeg[0]);
        int index = (int)(octdeg[1]);
        Index entry = 0;

        for (Index i = 0; i < this->mtof_tilde_01_innerScala_kbmMapSize; i++) {
            if (index == this->mtof_tilde_01_innerScala_kbmValid[(Index)(i + this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET)]) {
                entry = i;
                break;
            }
        }

        out = oct * this->mtof_tilde_01_innerScala_kbmSize + entry + this->mtof_tilde_01_innerScala_kbmMid;
    }

    out = out + frac;
    this->mtof_tilde_01_innerScala_updateLast(out, hz);
    return this->mtof_tilde_01_innerScala_lastNote;
}

int mtof_tilde_01_innerScala_updateScale(list scl) {
    if (scl->length < 1) {
        return 0;
    }

    number sclDataEntries = scl[0] * 2 + 1;

    if (sclDataEntries <= scl->length) {
        this->mtof_tilde_01_innerScala_lastValid = false;
        this->mtof_tilde_01_innerScala_sclExpMul = {};
        number last = 1;

        for (Index i = 1; i < sclDataEntries; i += 2) {
            const number c = (const number)(scl[(Index)(i + 0)]);
            const number d = (const number)(scl[(Index)(i + 1)]);

            if (d <= 0) {
                last = c / (number)1200;
            } else {
                last = rnbo_log2(c / d);
            }

            this->mtof_tilde_01_innerScala_sclExpMul->push(last);
        }

        this->mtof_tilde_01_innerScala_sclOctaveMul = last;
        this->mtof_tilde_01_innerScala_sclEntryCount = (int)(this->mtof_tilde_01_innerScala_sclExpMul->length);

        if (scl->length >= sclDataEntries + 3) {
            this->mtof_tilde_01_innerScala_kbmMid = (int)(scl[(Index)(sclDataEntries + 2)]);
            this->mtof_tilde_01_innerScala_kbmRefNum = (int)(scl[(Index)(sclDataEntries + 1)]);
            this->mtof_tilde_01_innerScala_kbmRefFreq = scl[(Index)(sclDataEntries + 0)];
            this->mtof_tilde_01_innerScala_kbmSize = (int)(0);
        }

        this->mtof_tilde_01_innerScala_updateRefFreq();
        return 1;
    }

    return 0;
}

int mtof_tilde_01_innerScala_updateMap(list kbm) {
    if (kbm->length == 1 && kbm[0] == 0.0) {
        kbm = {0.0, 0.0, 0.0, 60.0, 69.0, 440.0};
    }

    if (kbm->length >= 6 && kbm[0] >= 0.0) {
        this->mtof_tilde_01_innerScala_lastValid = false;
        Index size = (Index)(kbm[0]);
        int octave = 12;

        if (kbm->length > 6) {
            octave = (int)(kbm[6]);
        }

        if (size > 0 && kbm->length < this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET) {
            return 0;
        }

        this->mtof_tilde_01_innerScala_kbmSize = (int)(size);
        this->mtof_tilde_01_innerScala_kbmMin = (int)(kbm[1]);
        this->mtof_tilde_01_innerScala_kbmMax = (int)(kbm[2]);
        this->mtof_tilde_01_innerScala_kbmMid = (int)(kbm[3]);
        this->mtof_tilde_01_innerScala_kbmRefNum = (int)(kbm[4]);
        this->mtof_tilde_01_innerScala_kbmRefFreq = kbm[5];
        this->mtof_tilde_01_innerScala_kbmOctaveDegree = octave;
        this->mtof_tilde_01_innerScala_kbmValid = kbm;
        this->mtof_tilde_01_innerScala_kbmMapSize = (kbm->length - this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET > kbm->length ? kbm->length : (kbm->length - this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET < 0 ? 0 : kbm->length - this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET));
        this->mtof_tilde_01_innerScala_updateRefFreq();
        return 1;
    }

    return 0;
}

void mtof_tilde_01_innerScala_updateLast(number note, number freq) {
    this->mtof_tilde_01_innerScala_lastValid = true;
    this->mtof_tilde_01_innerScala_lastNote = note;
    this->mtof_tilde_01_innerScala_lastFreq = freq;
}

array<number, 2> mtof_tilde_01_innerScala_hztodeg(number hz) {
    number hza = rnbo_abs(hz);

    number octave = rnbo_floor(
        rnbo_log2(hza / this->mtof_tilde_01_innerScala_refFreq) / this->mtof_tilde_01_innerScala_sclOctaveMul
    );

    int i = 0;
    number frac = 0;
    number n = 0;

    for (; i < this->mtof_tilde_01_innerScala_sclEntryCount; i++) {
        number c = this->mtof_tilde_01_innerScala_applySCLOctIndex(octave, i + 0, 0.0, this->mtof_tilde_01_innerScala_refFreq);
        n = this->mtof_tilde_01_innerScala_applySCLOctIndex(octave, i + 1, 0.0, this->mtof_tilde_01_innerScala_refFreq);

        if (c <= hza && hza < n) {
            if (c != hza) {
                frac = rnbo_log2(hza / c) / rnbo_log2(n / c);
            }

            break;
        }
    }

    if (i == this->mtof_tilde_01_innerScala_sclEntryCount && n != hza) {
        number c = n;
        n = this->mtof_tilde_01_innerScala_applySCLOctIndex(octave + 1, 0, 0.0, this->mtof_tilde_01_innerScala_refFreq);
        frac = rnbo_log2(hza / c) / rnbo_log2(n / c);
    }

    number deg = i + octave * this->mtof_tilde_01_innerScala_sclEntryCount;

    {
        deg = rnbo_fround((deg + frac) * 1 / (number)1) * 1;
        frac = 0.0;
    }

    return {deg, frac};
}

array<int, 2> mtof_tilde_01_innerScala_octdegree(int degree, int count) {
    int octave = 0;
    int index = 0;

    if (degree < 0) {
        octave = -(1 + (-1 - degree) / count);
        index = -degree % count;

        if (index > 0) {
            index = count - index;
        }
    } else {
        octave = degree / count;
        index = degree % count;
    }

    return {octave, index};
}

array<int, 2> mtof_tilde_01_innerScala_applyKBM(number note) {
    if ((this->mtof_tilde_01_innerScala_kbmMin == this->mtof_tilde_01_innerScala_kbmMax && this->mtof_tilde_01_innerScala_kbmMax == 0) || (note >= this->mtof_tilde_01_innerScala_kbmMin && note <= this->mtof_tilde_01_innerScala_kbmMax)) {
        int degree = (int)(rnbo_floor(note - this->mtof_tilde_01_innerScala_kbmMid));

        if (this->mtof_tilde_01_innerScala_kbmSize == 0) {
            return {degree, 1};
        }

        array<int, 2> octdeg = this->mtof_tilde_01_innerScala_octdegree(degree, this->mtof_tilde_01_innerScala_kbmSize);
        int octave = (int)(octdeg[0]);
        Index index = (Index)(octdeg[1]);

        if (this->mtof_tilde_01_innerScala_kbmMapSize > index) {
            degree = (int)(this->mtof_tilde_01_innerScala_kbmValid[(Index)(this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET + index)]);

            if (degree >= 0) {
                return {degree + octave * this->mtof_tilde_01_innerScala_kbmOctaveDegree, 1};
            }
        }
    }

    return {-1, 0};
}

number mtof_tilde_01_innerScala_applySCL(int degree, number frac, number refFreq) {
    array<int, 2> octdeg = this->mtof_tilde_01_innerScala_octdegree(degree, this->mtof_tilde_01_innerScala_sclEntryCount);
    return this->mtof_tilde_01_innerScala_applySCLOctIndex(octdeg[0], octdeg[1], frac, refFreq);
}

number mtof_tilde_01_innerScala_applySCLOctIndex(number octave, int index, number frac, number refFreq) {
    number p = 0;

    if (index > 0) {
        p = this->mtof_tilde_01_innerScala_sclExpMul[(Index)(index - 1)];
    }

    if (frac > 0) {
        p = this->linearinterp(frac, p, this->mtof_tilde_01_innerScala_sclExpMul[(Index)index]);
    } else if (frac < 0) {
        p = this->linearinterp(-frac, this->mtof_tilde_01_innerScala_sclExpMul[(Index)index], p);
    }

    return refFreq * rnbo_pow(2, p + octave * this->mtof_tilde_01_innerScala_sclOctaveMul);
}

void mtof_tilde_01_innerScala_updateRefFreq() {
    this->mtof_tilde_01_innerScala_lastValid = false;
    int refOffset = (int)(this->mtof_tilde_01_innerScala_kbmRefNum - this->mtof_tilde_01_innerScala_kbmMid);

    if (refOffset == 0) {
        this->mtof_tilde_01_innerScala_refFreq = this->mtof_tilde_01_innerScala_kbmRefFreq;
    } else {
        int base = (int)(this->mtof_tilde_01_innerScala_kbmSize);

        if (base < 1) {
            base = this->mtof_tilde_01_innerScala_sclEntryCount;
        }

        array<int, 2> octdeg = this->mtof_tilde_01_innerScala_octdegree(refOffset, base);
        number oct = (number)(octdeg[0]);
        int index = (int)(octdeg[1]);

        if (base > 0) {
            oct = oct + rnbo_floor(index / base);
            index = index % base;
        }

        if (index >= 0 && index < this->mtof_tilde_01_innerScala_kbmSize) {
            if (index < this->mtof_tilde_01_innerScala_kbmMapSize) {
                index = (int)(this->mtof_tilde_01_innerScala_kbmValid[(Index)((Index)(index) + this->mtof_tilde_01_innerScala_KBM_MAP_OFFSET)]);
            } else {
                index = -1;
            }
        }

        if (index < 0 || index > this->mtof_tilde_01_innerScala_sclExpMul->length)
            {} else {
            number p = 0;

            if (index > 0) {
                p = this->mtof_tilde_01_innerScala_sclExpMul[(Index)(index - 1)];
            }

            this->mtof_tilde_01_innerScala_refFreq = this->mtof_tilde_01_innerScala_kbmRefFreq / rnbo_pow(2, p + oct * this->mtof_tilde_01_innerScala_sclOctaveMul);
        }
    }
}

void mtof_tilde_01_innerScala_reset() {
    this->mtof_tilde_01_innerScala_internal = true;
    this->mtof_tilde_01_innerScala_lastValid = false;
    this->mtof_tilde_01_innerScala_lastNote = 0;
    this->mtof_tilde_01_innerScala_lastFreq = 0;
    this->mtof_tilde_01_innerScala_sclEntryCount = 0;
    this->mtof_tilde_01_innerScala_sclOctaveMul = 1;
    this->mtof_tilde_01_innerScala_sclExpMul = {};
    this->mtof_tilde_01_innerScala_kbmValid = {0, 0, 0, 60, 69, 440};
    this->mtof_tilde_01_innerScala_kbmMid = 60;
    this->mtof_tilde_01_innerScala_kbmRefNum = 69;
    this->mtof_tilde_01_innerScala_kbmRefFreq = 440;
    this->mtof_tilde_01_innerScala_kbmSize = 0;
    this->mtof_tilde_01_innerScala_kbmMin = 0;
    this->mtof_tilde_01_innerScala_kbmMax = 0;
    this->mtof_tilde_01_innerScala_kbmOctaveDegree = 12;
    this->mtof_tilde_01_innerScala_kbmMapSize = 0;
    this->mtof_tilde_01_innerScala_refFreq = 261.63;
}

void mtof_tilde_01_init() {
    this->mtof_tilde_01_innerScala_update(this->mtof_tilde_01_scale, this->mtof_tilde_01_map);
}

void param_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_05_value;
}

void param_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_05_value_set(preset["value"]);
}

void ip_03_init() {
    this->ip_03_lastValue = this->ip_03_value;
}

void ip_03_fillSigBuf() {
    if ((bool)(this->ip_03_sigbuf)) {
        SampleIndex k = (SampleIndex)(this->sampleOffsetIntoNextAudioBuffer);

        if (k >= (SampleIndex)(this->vs))
            k = (SampleIndex)(this->vs) - 1;

        for (SampleIndex i = (SampleIndex)(this->ip_03_lastIndex); i < k; i++) {
            if (this->ip_03_resetCount > 0) {
                this->ip_03_sigbuf[(Index)i] = 1;
                this->ip_03_resetCount--;
            } else {
                this->ip_03_sigbuf[(Index)i] = this->ip_03_lastValue;
            }
        }

        this->ip_03_lastIndex = k;
    }
}

void ip_03_dspsetup(bool force) {
    if ((bool)(this->ip_03_setupDone) && (bool)(!(bool)(force)))
        return;

    this->ip_03_lastIndex = 0;
    this->ip_03_setupDone = true;
}

void param_06_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_06_value;
}

void param_06_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_06_value_set(preset["value"]);
}

void param_07_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_07_value;
}

void param_07_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_07_value_set(preset["value"]);
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
    expr_01_in1 = 0;
    expr_01_in2 = 1;
    expr_01_out1 = 0;
    param_01_value = 0;
    selector_01_onoff = 1;
    dspexpr_01_in1 = 0;
    dspexpr_01_in2 = 0;
    cycle_tilde_01_frequency = 0;
    cycle_tilde_01_phase_offset = 0;
    dspexpr_02_in1 = 0;
    dspexpr_02_in2 = 0;
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
    limi_02_bypass = 0;
    limi_02_dcblock = 0;
    limi_02_lookahead = 100;
    limi_02_lookahead_setter(limi_02_lookahead);
    limi_02_preamp = 0;
    limi_02_preamp_setter(limi_02_preamp);
    limi_02_postamp = 0;
    limi_02_postamp_setter(limi_02_postamp);
    limi_02_threshold = 0;
    limi_02_threshold_setter(limi_02_threshold);
    limi_02_release = 1000;
    svf_tilde_01_x = 0;
    svf_tilde_01_freq = 0;
    svf_tilde_01_q = 0;
    ip_01_value = 0;
    ip_01_impulse = 0;
    snapshot_01_interval = 1;
    snapshot_01_out = 0;
    rampsmooth_01_x = 0;
    rampsmooth_01_up = 0;
    rampsmooth_01_down = 0;
    intnum_01_input_number = 0;
    intnum_01_value = 0;
    param_02_value = 0;
    gen_01_in1 = 0;
    gen_01_rampsamples = 1;
    param_03_value = 150;
    ip_02_value = 0;
    ip_02_impulse = 0;
    param_04_value = 0.5;
    unpack_01_out1 = 0;
    unpack_01_out2 = 0;
    midiin_01_port = 0;
    mtof_tilde_01_midivalue = 0;
    mtof_tilde_01_base = 440;
    param_05_value = 0;
    ip_03_value = 0;
    ip_03_impulse = 0;
    expr_02_in1 = 0;
    expr_02_in2 = -12;
    expr_02_out1 = 0;
    param_06_value = 1;
    param_07_value = 0;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    signals[0] = nullptr;
    signals[1] = nullptr;
    signals[2] = nullptr;
    signals[3] = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    param_01_lastValue = 0;
    cycle_tilde_01_wrap = 0;
    cycle_tilde_01_ph_currentPhase = 0;
    cycle_tilde_01_ph_conv = 0;
    cycle_tilde_01_setupDone = false;
    limi_01_last = 0;
    limi_01_lookaheadIndex = 0;
    limi_01_recover = 0;
    limi_01_lookaheadInv = 0;
    limi_01_dc1_xm1 = 0;
    limi_01_dc1_ym1 = 0;
    limi_01_setupDone = false;
    limi_02_last = 0;
    limi_02_lookaheadIndex = 0;
    limi_02_recover = 0;
    limi_02_lookaheadInv = 0;
    limi_02_dc1_xm1 = 0;
    limi_02_dc1_ym1 = 0;
    limi_02_setupDone = false;
    svf_tilde_01_low = 0;
    svf_tilde_01_high = 0;
    svf_tilde_01_band = 0;
    svf_tilde_01_notch = 0;
    svf_tilde_01__freq = 0.001;
    svf_tilde_01__q = 0.001;
    svf_tilde_01_dFreq_prev = 0;
    svf_tilde_01_dQ_prev = 0;
    svf_tilde_01_setupDone = false;
    ip_01_lastIndex = 0;
    ip_01_lastValue = 0;
    ip_01_resetCount = 0;
    ip_01_sigbuf = nullptr;
    ip_01_setupDone = false;
    snapshot_01_calc = 0;
    snapshot_01_nextTime = 0;
    snapshot_01_count = 0;
    snapshot_01_lastValue = 0;
    rampsmooth_01_prev = 0;
    rampsmooth_01_index = 0;
    rampsmooth_01_increment = 0;
    rampsmooth_01_d_prev = 0;
    intnum_01_stored = 0;
    param_02_lastValue = 0;
    gen_01_history_1_value = 0;
    gen_01_history_2_value = 0;
    gen_01_history_3_value = 0;
    gen_01_latch_5_value = 0;
    gen_01_latch_7_value = 0;
    gen_01_counter_10_carry = 0;
    gen_01_counter_10_count = 0;
    gen_01_setupDone = false;
    param_03_lastValue = 0;
    ip_02_lastIndex = 0;
    ip_02_lastValue = 0;
    ip_02_resetCount = 0;
    ip_02_sigbuf = nullptr;
    ip_02_setupDone = false;
    param_04_lastValue = 0;
    midiparse_01_parser_status = -1;
    midiparse_01_parser_byte1 = -1;
    mtof_tilde_01_innerMtoF_lastInValue = 0;
    mtof_tilde_01_innerMtoF_lastOutValue = 0;
    mtof_tilde_01_innerMtoF_lastTuning = 0;
    mtof_tilde_01_innerScala_internal = true;
    mtof_tilde_01_innerScala_lastValid = false;
    mtof_tilde_01_innerScala_lastNote = 0;
    mtof_tilde_01_innerScala_lastFreq = 0;
    mtof_tilde_01_innerScala_sclEntryCount = 0;
    mtof_tilde_01_innerScala_sclOctaveMul = 1;
    mtof_tilde_01_innerScala_kbmValid = { 0, 0, 0, 60, 69, 440 };
    mtof_tilde_01_innerScala_kbmMid = 60;
    mtof_tilde_01_innerScala_kbmRefNum = 69;
    mtof_tilde_01_innerScala_kbmRefFreq = 440;
    mtof_tilde_01_innerScala_kbmSize = 0;
    mtof_tilde_01_innerScala_kbmMin = 0;
    mtof_tilde_01_innerScala_kbmMax = 0;
    mtof_tilde_01_innerScala_kbmOctaveDegree = 12;
    mtof_tilde_01_innerScala_kbmMapSize = 0;
    mtof_tilde_01_innerScala_refFreq = 261.63;
    param_05_lastValue = 0;
    ip_03_lastIndex = 0;
    ip_03_lastValue = 0;
    ip_03_resetCount = 0;
    ip_03_sigbuf = nullptr;
    ip_03_setupDone = false;
    param_06_lastValue = 0;
    param_07_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_state = nullptr;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number expr_01_in1;
    number expr_01_in2;
    number expr_01_out1;
    number param_01_value;
    number selector_01_onoff;
    number dspexpr_01_in1;
    number dspexpr_01_in2;
    number cycle_tilde_01_frequency;
    number cycle_tilde_01_phase_offset;
    number dspexpr_02_in1;
    number dspexpr_02_in2;
    number limi_01_bypass;
    number limi_01_dcblock;
    number limi_01_lookahead;
    number limi_01_preamp;
    number limi_01_postamp;
    number limi_01_threshold;
    number limi_01_release;
    number limi_02_bypass;
    number limi_02_dcblock;
    number limi_02_lookahead;
    number limi_02_preamp;
    number limi_02_postamp;
    number limi_02_threshold;
    number limi_02_release;
    number svf_tilde_01_x;
    number svf_tilde_01_freq;
    number svf_tilde_01_q;
    number ip_01_value;
    number ip_01_impulse;
    number snapshot_01_interval;
    number snapshot_01_out;
    number rampsmooth_01_x;
    number rampsmooth_01_up;
    number rampsmooth_01_down;
    number intnum_01_input_number;
    number intnum_01_value;
    number param_02_value;
    number gen_01_in1;
    number gen_01_rampsamples;
    number param_03_value;
    number ip_02_value;
    number ip_02_impulse;
    number param_04_value;
    number unpack_01_out1;
    number unpack_01_out2;
    number midiin_01_port;
    number mtof_tilde_01_midivalue;
    list mtof_tilde_01_scale;
    list mtof_tilde_01_map;
    number mtof_tilde_01_base;
    number param_05_value;
    number ip_03_value;
    number ip_03_impulse;
    number expr_02_in1;
    number expr_02_in2;
    number expr_02_out1;
    number param_06_value;
    number param_07_value;
    MillisecondTime _currentTime;
    UInt64 audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    SampleValue * signals[4];
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    number param_01_lastValue;
    Float64BufferRef cycle_tilde_01_buffer;
    long cycle_tilde_01_wrap;
    uint32_t cycle_tilde_01_phasei;
    SampleValue cycle_tilde_01_f2i;
    number cycle_tilde_01_ph_currentPhase;
    number cycle_tilde_01_ph_conv;
    bool cycle_tilde_01_setupDone;
    SampleValue limi_01_lookaheadBuffers[1][512] = { };
    SampleValue limi_01_gainBuffer[512] = { };
    number limi_01_last;
    int limi_01_lookaheadIndex;
    number limi_01_recover;
    number limi_01_lookaheadInv;
    number limi_01_dc1_xm1;
    number limi_01_dc1_ym1;
    bool limi_01_setupDone;
    SampleValue limi_02_lookaheadBuffers[1][512] = { };
    SampleValue limi_02_gainBuffer[512] = { };
    number limi_02_last;
    int limi_02_lookaheadIndex;
    number limi_02_recover;
    number limi_02_lookaheadInv;
    number limi_02_dc1_xm1;
    number limi_02_dc1_ym1;
    bool limi_02_setupDone;
    number svf_tilde_01_low;
    number svf_tilde_01_high;
    number svf_tilde_01_band;
    number svf_tilde_01_notch;
    number svf_tilde_01__freq;
    number svf_tilde_01__q;
    number svf_tilde_01_dFreq_prev;
    number svf_tilde_01_dQ_prev;
    bool svf_tilde_01_setupDone;
    SampleIndex ip_01_lastIndex;
    number ip_01_lastValue;
    SampleIndex ip_01_resetCount;
    signal ip_01_sigbuf;
    bool ip_01_setupDone;
    number snapshot_01_calc;
    number snapshot_01_nextTime;
    SampleIndex snapshot_01_count;
    number snapshot_01_lastValue;
    number rampsmooth_01_prev;
    number rampsmooth_01_index;
    number rampsmooth_01_increment;
    number rampsmooth_01_d_prev;
    number intnum_01_stored;
    number param_02_lastValue;
    number gen_01_history_1_value;
    number gen_01_history_2_value;
    number gen_01_history_3_value;
    number gen_01_latch_5_value;
    number gen_01_latch_7_value;
    int gen_01_counter_10_carry;
    number gen_01_counter_10_count;
    bool gen_01_setupDone;
    number param_03_lastValue;
    SampleIndex ip_02_lastIndex;
    number ip_02_lastValue;
    SampleIndex ip_02_resetCount;
    signal ip_02_sigbuf;
    bool ip_02_setupDone;
    number param_04_lastValue;
    int midiparse_01_parser_status;
    int midiparse_01_parser_byte1;
    number mtof_tilde_01_innerMtoF_lastInValue;
    number mtof_tilde_01_innerMtoF_lastOutValue;
    number mtof_tilde_01_innerMtoF_lastTuning;
    Float64BufferRef mtof_tilde_01_innerMtoF_buffer;
    bool mtof_tilde_01_innerScala_internal;
    const Index mtof_tilde_01_innerScala_KBM_MAP_OFFSET = 7;
    bool mtof_tilde_01_innerScala_lastValid;
    number mtof_tilde_01_innerScala_lastNote;
    number mtof_tilde_01_innerScala_lastFreq;
    int mtof_tilde_01_innerScala_sclEntryCount;
    number mtof_tilde_01_innerScala_sclOctaveMul;
    list mtof_tilde_01_innerScala_sclExpMul;
    list mtof_tilde_01_innerScala_kbmValid;
    int mtof_tilde_01_innerScala_kbmMid;
    int mtof_tilde_01_innerScala_kbmRefNum;
    number mtof_tilde_01_innerScala_kbmRefFreq;
    int mtof_tilde_01_innerScala_kbmSize;
    int mtof_tilde_01_innerScala_kbmMin;
    int mtof_tilde_01_innerScala_kbmMax;
    int mtof_tilde_01_innerScala_kbmOctaveDegree;
    Index mtof_tilde_01_innerScala_kbmMapSize;
    number mtof_tilde_01_innerScala_refFreq;
    number param_05_lastValue;
    SampleIndex ip_03_lastIndex;
    number ip_03_lastValue;
    SampleIndex ip_03_resetCount;
    signal ip_03_sigbuf;
    bool ip_03_setupDone;
    number param_06_lastValue;
    number param_07_lastValue;
    signal globaltransport_tempo;
    signal globaltransport_state;
    number stackprotect_count;
    DataRef RNBODefaultSinus;
    DataRef RNBODefaultMtofLookupTable256;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* createsubgen()
{
    return new subgen();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr subgenFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return createsubgen;
}

} // end RNBO namespace

