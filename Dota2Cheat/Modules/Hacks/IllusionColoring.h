#pragma once
#include "../../CheatSDK/include.h"
#include "../MListeners.h"

namespace Modules {

	inline class M_IllusionESP : public IEntityListListener, public IFrameListener {
		std::set<CDOTABaseNPC_Hero*> IllusionCandidates;
		std::set<CDOTABaseNPC_Hero*> LancerIllusions; // Separately colored

		void ClientAddNewModifier(CDOTABaseNPC_Hero* hero, const char* modifierName);
	public:

		void OnEntityAdded(const EntityWrapper& ent) override {
			if (ent.type != EntityType::Hero)
				return;

			std::string_view name = ent->SchemaBinding()->binaryName;

			// Not considering their clones illusions
			if (name == "CDOTA_Unit_Hero_ArcWarden" ||
				name == "C_DOTA_Unit_Hero_Meepo")
				return;
			if (name == "C_DOTA_Unit_Hero_PhantomLancer")
				LancerIllusions.insert(ent);
			else
				IllusionCandidates.insert(ent);
		}
		void OnEntityRemoved(const EntityWrapper& ent) override {
			IllusionCandidates.erase(ent);
			LancerIllusions.erase(ent);
		}
		void ColorLancerIllusion(CDOTABaseNPC_Hero* hero) {
			if (ctx.localHero->IsSameTeam(hero) ||
				!hero->IsIllusion())
				return;

			auto& isSeenAsIllusion = hero->Field<bool>(Netvars::C_DOTA_BaseNPC::m_bHasClientSeenIllusionModifier);

			const auto& m_clrRender = Config::IllusionColoring::Color;

			if (!isSeenAsIllusion) {
				// from CDOTA_Modifier_Illusion::OnCreated
				// also observed via ��
				ClientAddNewModifier(hero, "modifier_phantom_lancer_juxtapose_illusion");
				isSeenAsIllusion = true;
			}
		}
		void ColorIllusion(CDOTABaseNPC_Hero* hero);
		void OnFrame() override {
			if (!Config::IllusionColoring::Enabled)
				return;
			for (auto illusion : IllusionCandidates)
				ColorIllusion(illusion);
			for (auto illusion : LancerIllusions)
				ColorLancerIllusion(illusion);
		}

		M_IllusionESP() {
			IEntityListListener::Subscribe(this);
		}
	} IllusionESP{};
}