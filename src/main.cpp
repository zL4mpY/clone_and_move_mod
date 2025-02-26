#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

class $modify(MyEditorUI, EditorUI) {
	void on_buttons_click(CCObject* sender) {
		const std::array<std::string, 4> buttonIDs = {
			"move-up-button", 
			"move-down-button", 
			"move-left-button", 
			"move-right-button"
		};

        if (this->getSelectedObjects()->count()) {
            auto* objects = this->getSelectedObjects();
            m_copyPasteBtn->activate(); 
				
			auto editButtons = m_editButtonBar->m_buttonArray;
			auto buttonID = buttonIDs[sender->getTag() - 1];
			
			if (this->querySelector("hjfod.betteredit/custom-move-menu")) {
				auto beMoveMenu = this->getChildByID("hjfod.betteredit/custom-move-menu");
				auto* moveButton = static_cast<CCMenuItem*>(beMoveMenu->getChildByIDRecursive(buttonID));
				moveButton->activate();
			} else {
				auto* moveButton = static_cast<CCMenuItem*>(m_editButtonBar->getChildByIDRecursive(buttonID));
				moveButton->activate();
			}
        }
    }

	void createMoveMenu() {
		EditorUI::createMoveMenu();

		const std::array<float, 4> rotations = {0, 180, 270, 90};
		const std::array<std::string, 4> buttonIDs = {
			"move-up-move-clone-button"_spr, 
			"move-down-move-clone-button"_spr, 
			"move-left-move-clone-button"_spr, 
			"move-right-move-clone-button"_spr
		};

		for (int i = 0; i < 4; i++) {
		    auto* copyAndMoveBtn = this->getSpriteButton("button.png"_spr, menu_selector(MyEditorUI::on_buttons_click), nullptr, 0.9f);
		    copyAndMoveBtn->setRotation(rotations[i]);
		    copyAndMoveBtn->setTag(i + 1);
			copyAndMoveBtn->setID(buttonIDs[i]);
		    m_editButtonBar->m_buttonArray->addObject(copyAndMoveBtn);
		}

		auto rows = GameManager::sharedState()->getIntGameVariable("0049");
		auto cols = GameManager::sharedState()->getIntGameVariable("0050");

		m_editButtonBar->reloadItems(rows, cols);
	}
};