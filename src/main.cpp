#include "Geode/loader/SettingV3.hpp"
#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

const std::array<float, 4> rotations = {0, 180, 270, 90};

std::array<char const*, 4> spriteIDs = {
	"clone_and_move_upBtn.png"_spr,
	"clone_and_move_downBtn.png"_spr,
	"clone_and_move_leftBtn.png"_spr,
	"clone_and_move_rightBtn.png"_spr
};

std::array<std::string, 4> buttonIDs = {
	"move-up-move-clone-button"_spr, 
	"move-down-move-clone-button"_spr, 
	"move-left-move-clone-button"_spr, 
	"move-right-move-clone-button"_spr
};

std::string requiredButtonId = "move-right-tiny-button";
std::string relocateButtonsType = "Default";
bool isRelocateButtonsModInstalled = false;
std::map<std::string, int> relocateButtonsTypes;
std::vector<CCMenuItemSpriteExtra*> modButtons;
float moveStep = 1.0f;

$execute {
	relocateButtonsTypes.insert(std::map<std::string, int>::value_type("Before Half Move Buttons", 0));
	relocateButtonsTypes.insert(std::map<std::string, int>::value_type("Default", 1));
	relocateButtonsTypes.insert(std::map<std::string, int>::value_type("After Move Unit Buttons", 2));
	relocateButtonsTypes.insert(std::map<std::string, int>::value_type("End", 3));

	relocateButtonsType = Mod::get()->getSettingValue<std::string>("buttons-before-half-buttons");
	int relocateButtonsTypeValue = relocateButtonsTypes.at(relocateButtonsType);

	switch (relocateButtonsTypeValue) {
		case 0:
			requiredButtonId = "move-right-tiny-button";
			break;
		
		case 2:
			requiredButtonId = "hjfod.betteredit/move-right-unit-button";
			break;
	}

	moveStep = Mod::get()->getSettingValue<float>("move-step");

	listenForSettingChanges<float>("move-step", [](float value) {
		moveStep = value;
	});
	
    listenForSettingChanges<std::string>("buttons-before-half-buttons", [](std::string value) {
		relocateButtonsType = value;
		int relocateButtonsTypeValue = relocateButtonsTypes.at(relocateButtonsType);

		switch (relocateButtonsTypeValue) {
			case 0:
				requiredButtonId = "move-right-tiny-button";
				break;
			
			case 2:
				requiredButtonId = "hjfod.betteredit/move-right-unit-button";
				break;
		}
    });

	if (Loader::get()->isModLoaded("loovigmd.relocate_edit_buttons")) isRelocateButtonsModInstalled = true;
}

class $modify(MyEditorUI, EditorUI) {
	static void onModify(auto& self) {
		if (!self.setHookPriorityAfterPost("EditorUI::init", "hjfod.betteredit")) {
            geode::log::warn("Failed to set hook priority.");
        }
		
	}
	struct Fields {
		~Fields() {
			modButtons.clear();
		}
	};

	$override
	bool init(LevelEditorLayer* editorLayer) {
		if (!EditorUI::init(editorLayer)) {
			return false;
		}

		this->createKeybinds();

		for (int i = 0; i < 4; i++) {
			auto* copyAndMoveBtn = this->getSpriteButton(spriteIDs[i], menu_selector(MyEditorUI::onButtonsClick), nullptr, 0.9f);
			copyAndMoveBtn->setTag(i + 1);
			copyAndMoveBtn->setID(buttonIDs[i]);
			m_editButtonBar->m_buttonArray->addObject(copyAndMoveBtn);
			modButtons.push_back(copyAndMoveBtn);
		}

		auto* modSettingsBtn = this->getSpriteButton("clone_and_move_optionsBtn.png"_spr, menu_selector(MyEditorUI::onSettingsPopup), nullptr, 1.0f);
		modSettingsBtn->setID("options-move-clone-button"_spr);
		m_editButtonBar->m_buttonArray->addObject(modSettingsBtn);
		modButtons.push_back(modSettingsBtn);

		auto rows = GameManager::sharedState()->getIntGameVariable("0049");
		auto cols = GameManager::sharedState()->getIntGameVariable("0050");
		
		if (relocateButtonsType != "Default") {
			this->relocateModButtons();
		}

		m_editButtonBar->loadFromItems(m_editButtonBar->m_buttonArray, rows, cols, true);

		this->m_fields.self();
		return true;
	}

	void relocateModButtons() {
		int requiredButtonIndex = -1;
		
		if (relocateButtonsType == "End") {
			requiredButtonIndex = m_editButtonBar->m_buttonArray->count() - 1 - modButtons.size();
		} else {
			for (int i = 0; i < m_editButtonBar->m_buttonArray->count(); i++) {
				auto button = static_cast<CCNode*>(m_editButtonBar->m_buttonArray->objectAtIndex(i));
				if (button->getID() == requiredButtonId) {
					requiredButtonIndex = i;
					break;
				}
			}
		}

		if (requiredButtonIndex >= 0) {
			CCArray *modifiedItems = CCArray::create();
			
			for (auto* modBtn : modButtons) {
				m_editButtonBar->m_buttonArray->removeObject(modBtn);
			}

			for (int i = 0; i < m_editButtonBar->m_buttonArray->count(); i++) {
				auto button = static_cast<CCNode*>(m_editButtonBar->m_buttonArray->objectAtIndex(i));
				modifiedItems->addObject(button);
				
				if (i == requiredButtonIndex) {
					if (isRelocateButtonsModInstalled) {
						for (auto* modBtn : std::vector<CCMenuItemSpriteExtra*>(modButtons.begin(), modButtons.end()-1)) {
							modifiedItems->addObject(modBtn);
						}
					} else {
						for (auto* modBtn : modButtons) {
							modifiedItems->addObject(modBtn);
						}
					}
				}
			}

			if (isRelocateButtonsModInstalled) modifiedItems->addObject(modButtons.back());

			m_editButtonBar->m_buttonArray->removeAllObjects();
			m_editButtonBar->m_buttonArray->addObjectsFromArray(modifiedItems);
		}
	}

	void createKeybinds() {
		this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), "clone-and-move-obj-up-keybind"),
            [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (down && !repeat) {
                    cloneAndMoveObjects(0);
                }
            }
        );

		this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), "clone-and-move-obj-down-keybind"),
            [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (down && !repeat) {
                    cloneAndMoveObjects(1);
                }
            }
        );

		this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), "clone-and-move-obj-left-keybind"),
            [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (down && !repeat) {
                    cloneAndMoveObjects(2);
                }
            }
        );

		this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), "clone-and-move-obj-right-keybind"),
            [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (down && !repeat) {
                    cloneAndMoveObjects(3);
                }
            }
        );
	}

	void cloneAndMoveObjects(int direction) {
		// Creating an array for finding the required button
		const std::array<std::string, 4> buttonIDs = {
			"move-up-button", 
			"move-down-button", 
			"move-left-button", 
			"move-right-button"
		};

		// If there are objects selected, the function continues
        if (this->getSelectedObjects()->count()) {
			float gridSize;

			/*
				If Better Move Buttons Menu exists then we get its step size from its text input object
				In case if that mod gets accepted to the index because it is cool
				(Not included for now because I will probably update it and it also started crashing for no reason)
			*/
			/* if (this->querySelector("better-move-buttons-menu")) {
			   	auto* betterMoveMenu = static_cast<CCMenu*>(this->querySelector("better-move-buttons-menu"));
			   	auto stepInput = static_cast<TextInput*>(betterMoveMenu->getChildByType<TextInput>(0));
			   	gridSize = std::stof(stepInput->getString());

			   // Else we get the grid size of editor
			   } else {
			   	this->updateGridNodeSize();
			   	gridSize = this->m_gridSize;
			   }
			*/

			this->updateGridNodeSize();
			gridSize = this->m_gridSize;

			auto objects = this->getSelectedObjects();
			std::string objectStrings = "";

			float deltaX = 0;
			float deltaY = 0;

			// Get delta coordinates
			switch (direction) {
				case 0:
					deltaY = gridSize;
					break;

				case 1:
					deltaY = -gridSize;
					break;
				
				case 2:
					deltaX = -gridSize;
					break;
				
				case 3:
					deltaX = gridSize;
					break;
				
				default:
					break;
			}

			deltaX *= moveStep;
			deltaY *= moveStep;

			/*
				Copying the selected objects, saving their strings first
				Code credits to undefined06855 on Geode Discord for this block of code
			*/
			for (int i = 0 ; i < objects->count() ; i++) {
				auto* object = typeinfo_cast<GameObject*>(objects->objectAtIndex(i));
				
				// Doing this check since casting StartPos to GameObject crashes the mod
				if (!object) {
					object = typeinfo_cast<StartPosObject*>(objects->objectAtIndex(i));
					if (!object) {
						FLAlertLayer::create("Error", "Unknown object type found. Please let the developer know about this bug.", "OK")->show();
						return;
					}
				}

				auto objectString = static_cast<std::string>(object->getSaveString(GJBaseGameLayer::get())) + ";";
				objectStrings += objectString;
			}

			// Create the copied objects
			auto levelEditorLayer = LevelEditorLayer::get();
			auto copiedObjects = levelEditorLayer->createObjectsFromString(gd::string(objectStrings), true, true);
			auto copiedObjects2 = CCArray::create();

			for (int i = 0 ; i < copiedObjects->count() ; i++) {
				auto* object = typeinfo_cast<GameObject*>(copiedObjects->objectAtIndex(i));
				
				// Adding the check here too
				if (!object) {
					object = typeinfo_cast<StartPosObject*>(copiedObjects->objectAtIndex(i));
					if (!object) {
						FLAlertLayer::create("Error", "Unknown object type found. Please let the developer know about this bug.", "OK")->show();
						return;
					}
				}

				auto objectPosition = object->getPosition();
				object->setPosition(objectPosition + CCPoint{deltaX, deltaY}); // Moving copied objects
				copiedObjects2->addObject(object);
			}

			// Deselect previous objects and then selecting the copied ones
			this->deselectAll();
			levelEditorLayer->m_undoObjects->addObject(UndoObject::createWithArray(copiedObjects2, UndoCommand::Paste));
			this->selectObjects(copiedObjects2, true);

			// Recoloring them so they look like they are copypasted
			for (int i = 0 ; i < copiedObjects2->count() ; i++) {
				auto* object = typeinfo_cast<GameObject*>(copiedObjects2->objectAtIndex(i));

				// And adding the check here
				if (!object) {
					object = typeinfo_cast<StartPosObject*>(copiedObjects2->objectAtIndex(i));
					if (!object) {
						FLAlertLayer::create("Error", "Unknown object type found. Please let the developer know about this bug.", "OK")->show();
						return;
					}
				}

				object->updateMainColor(ccColor3B{0, 255, 255});
				object->updateSecondaryColor(ccColor3B{0, 204, 204});
			}
			
			this->updateButtons();
			this->updateObjectInfoLabel();
		}
	}

	void onButtonsClick(CCObject* sender) {
		cloneAndMoveObjects(sender->getTag()-1);
    }

	void onSettingsPopup(CCObject* sender) {
		openSettingsPopup(Mod::get());
	}
};


// // New moving buttons logic made with help from kuel27! Thank you!
// class $modify(EditButtonBar) {
//     $override
// 	void loadFromItems(CCArray *items, int r, int c, bool unkBool) {
// 		if (relocateButtonsType != "Default") {
// 			if (this->getID() == "edit-tab-bar") {
//     		    if (modButtons.size() > 0) {
//     		        int requiredButtonIndex = -1;
// 					if (relocateButtonsType == "End") {
// 						requiredButtonIndex = items->count() - 1;
// 					}

// 					if (requiredButtonIndex == -1) {
//     		    	    for (int i = 0; i < items->count(); i++) {
//     		    	        auto button = static_cast<CCNode*>(items->objectAtIndex(i));
						
//     		    	        if (button->getID() == requiredButtonId) {
//     		    	            requiredButtonIndex = i;
//     		    	            break;
//     		    	        }
//     		    	    }
// 					}

//     		        if (requiredButtonIndex >= 0) {
//     		            CCArray *modifiedItems = CCArray::create();

//     		            for (int i = 0; i < items->count(); i++) {
//     		                auto button = static_cast<CCNode *>(items->objectAtIndex(i));

//     		                bool shouldRemove = false;
//     		                for (const auto &id : buttonIDs) {
//     		                    if (button->getID() == id) {
//     		                        shouldRemove = true;
//     		                        break;
//     		                    }
//     		                }

//     		                if (!shouldRemove) {
//     		                    modifiedItems->addObject(button);
//     		                }

//     		                if (i == requiredButtonIndex) {
// 								// for (int j = 0 ; j < modButtons.size() ; j++) {
// 								// 	auto button = modButtons.at(j);
//     		                    // 	modifiedItems->addObject(button);
// 								// }

// 								for (auto* modBtn : modButtons) {
// 									if (modBtn->getParent()) {
// 										modBtn->removeFromParentAndCleanup(false);
// 									}
// 									modifiedItems->addObject(modBtn);
// 								}
//     		                }
//     		            }

// 						this->m_buttonArray->removeAllObjects();
// 						this->m_buttonArray->addObjectsFromArray(modifiedItems);

//     		            EditButtonBar::loadFromItems(modifiedItems, r, c, unkBool);
//     		            return;
//     		        } else {
//     		            log::warn("Required button '{}' not found, using original layout", requiredButtonId);
//     		        }
//     		    }
// 			}
// 		}

//         EditButtonBar::loadFromItems(items, r, c, unkBool);
//     }
// };