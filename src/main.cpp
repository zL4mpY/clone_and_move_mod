#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>

class $modify(MyEditorUI, EditorUI) {
	void on_buttons_click(CCObject* sender) {
		// Creating an array for finding the required button
		const std::array<std::string, 4> buttonIDs = {
			"move-up-button", 
			"move-down-button", 
			"move-left-button", 
			"move-right-button"
		};

		// If there are objects selected, the function continues
        if (this->getSelectedObjects()->count()) {
			// Activating Copy+Paste button
            m_copyPasteBtn->activate(); 
				
			auto editButtons = m_editButtonBar->m_buttonArray;

			// Getting the required move button ID
			auto buttonID = buttonIDs[sender->getTag() - 1];
			
			if (this->querySelector("hjfod.betteredit/custom-move-menu")) {
				auto beMoveMenu = this->getChildByID("hjfod.betteredit/custom-move-menu");

				// Getting the required move button and activating it
				auto* moveButton = static_cast<CCMenuItem*>(beMoveMenu->getChildByIDRecursive(buttonID));
				moveButton->activate();
			} else {
				// The same action but if BetterEdit is not found
				auto* moveButton = static_cast<CCMenuItem*>(m_editButtonBar->getChildByIDRecursive(buttonID));
				moveButton->activate();
			}
        }
    }

	void createMoveMenu() {
		EditorUI::createMoveMenu();

		// Creating an array of required values for buttons
		const std::array<float, 4> rotations = {0, 180, 270, 90};
		const std::array<std::string, 4> buttonIDs = {
			"move-up-move-clone-button"_spr, 
			"move-down-move-clone-button"_spr, 
			"move-left-move-clone-button"_spr, 
			"move-right-move-clone-button"_spr
		};

		// Adding 4 mod buttons here
		for (int i = 0; i < 4; i++) {
		    auto* copyAndMoveBtn = this->getSpriteButton("clone_and_move_button.png"_spr, menu_selector(MyEditorUI::on_buttons_click), nullptr, 0.9f);
		    copyAndMoveBtn->setRotation(rotations[i]);

			// Setting a tag so we can check which button is being pressed
		    copyAndMoveBtn->setTag(i + 1);
			copyAndMoveBtn->setID(buttonIDs[i]);

			// Adding the button to the Edit tab
		    m_editButtonBar->m_buttonArray->addObject(copyAndMoveBtn);
		}

		auto rows = GameManager::sharedState()->getIntGameVariable("0049");
		auto cols = GameManager::sharedState()->getIntGameVariable("0050");
		
		// Updating the Edit tab
		m_editButtonBar->reloadItems(rows, cols);
	}
};