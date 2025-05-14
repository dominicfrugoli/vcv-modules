#include "plugin.hpp"


struct Planky : Module {
	enum ParamId {
		FREQ_PARAM,
		OSC1_PARAM,
		OSC2_PARAM,
		OSC3_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		FREQ_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OSC1_OUTPUT,
		OSC2_OUTPUT,
		OSC3_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		OSC1_LIGHT_LIGHT,
		OSC2_LIGHT_LIGHT,
		OSC3_LIGHT_LIGHT,
		LIGHTS_LEN
	};

	Planky() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FREQ_PARAM, 0.f, 1.f, 0.f, "Master Frequency");
		configParam(OSC1_PARAM, 0.f, 1.f, 0.f, "Oscillator 1 Offset");
		configParam(OSC2_PARAM, 0.f, 1.f, 0.f, "Oscillator 2 Offset");
		configParam(OSC3_PARAM, 0.f, 1.f, 0.f, "Oscillator 3 Offset");
		configInput(FREQ_INPUT, "Master Frequency CV");
		configOutput(OSC1_OUTPUT, "Oscillator 1 Output");
		configOutput(OSC2_OUTPUT, "Oscillator 2 Output");
		configOutput(OSC3_OUTPUT, "Oscillator 3 Output");
	}

	float oscPhases[3] = {0.0f, 0.0f, 0.0f};
	float sines[3] = {0.0f, 0.0f, 0.0f};

	void process(const ProcessArgs& args) override
	{
		// Compute the frequency from the pitch parameter and input
		float pitch = params[FREQ_PARAM].getValue();
		pitch += inputs[FREQ_INPUT].getVoltage();
		// The default frequency is C4 = 261.6256f
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

		
		
		// Audio signals are typically +/-5V
		// https://vcvrack.com/manual/VoltageStandards
		//outputs[SINE_OUTPUT].setVoltage(5.f * sine);
		for(int i = 0; i < 3; i++)
		{
			// Accumulate the phase
			oscPhases[i] += freq * args.sampleTime;
			if (oscPhases[i] >= 1.f)
				oscPhases[i] -= 1.f;

			// Compute the sine output
			sines[i] = std::sin(2.f * M_PI * oscPhases[i]);


			outputs[i].setVoltage(5.f * sines[i]);
		}
	}
};


struct PlankyWidget : ModuleWidget {
	PlankyWidget(Planky* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Planky.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.5, 30.63)), module, Planky::FREQ_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6, 79.493)), module, Planky::OSC1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.517, 79.493)), module, Planky::OSC2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(55, 79.493)), module, Planky::OSC3_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.513, 54.944)), module, Planky::FREQ_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(18.3, 112.431)), module, Planky::OSC1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.48, 112.431)), module, Planky::OSC2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(42.7, 112.431)), module, Planky::OSC3_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(6, 91.818)), module, Planky::OSC1_LIGHT_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.517, 91.818)), module, Planky::OSC2_LIGHT_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(56, 91.818)), module, Planky::OSC3_LIGHT_LIGHT));
	}
};


Model* modelPlanky = createModel<Planky, PlankyWidget>("Planky");