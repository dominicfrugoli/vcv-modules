#include "plugin.hpp"


struct Plank : Module
{
	enum ParamId {
		FREQ_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		FREQ_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		SINE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		BLINK_LIGHT,
		LIGHTS_LEN
	};

	Plank() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FREQ_PARAM, 0.f, 1.f, 0.f, "Frequency Knob");
		configInput(FREQ_INPUT, "Frequency CV");
		configOutput(SINE_OUTPUT, "Sine Output");
	}

	float phase = 0.f;
	float blinkPhase = 0.f;

	void process(const ProcessArgs& args) override
	{
		// Compute the frequency from the pitch parameter and input
		float pitch = params[FREQ_PARAM].getValue();
		pitch += inputs[FREQ_INPUT].getVoltage();
		// The default frequency is C4 = 261.6256f
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

		// Accumulate the phase
		phase += freq * args.sampleTime;
		if (phase >= 1.f)
			phase -= 1.f;

		// Compute the sine output
		float sine = std::sin(2.f * M_PI * phase);
		// Audio signals are typically +/-5V
		// https://vcvrack.com/manual/VoltageStandards
		outputs[SINE_OUTPUT].setVoltage(5.f * sine);

		// Blink light at 1Hz
		blinkPhase += args.sampleTime;
		if (blinkPhase >= 1.f)
			blinkPhase -= 1.f;
		lights[BLINK_LIGHT].setBrightness(blinkPhase < 0.5f ? 1.f : 0.f);
	}
};


struct PlankWidget : ModuleWidget
{
	PlankWidget(Plank* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/plank.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.48, 34.674)), module, Plank::FREQ_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.48, 91.375)), module, Plank::FREQ_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.48, 112.431)), module, Plank::SINE_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.48, 60.805)), module, Plank::BLINK_LIGHT));
	}
};


Model* modelPlank = createModel<Plank, PlankWidget>("plank");