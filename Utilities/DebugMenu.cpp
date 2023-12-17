#include "DebugMenu.h"
#include<ImGui/backends/imgui_impl_glfw.h>
#include<ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui-knobs.h>

DebugMenu::DebugMenu(std::vector<GameObject>& gameObjects, LightSystem& lightSystem, bool& editMode, GLFWwindow* window)
	:m_gameObjects(gameObjects),
	m_editMode(editMode),
	m_LightSystem(lightSystem)
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	const char* glsl_version = "#version 460";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	SetObjectMenu(gameObjects[0]);
	SetDirectionalLightMenu();
	m_SelectedPointLight = lightSystem.GetPointLightList()[0].Name;
}

DebugMenu::~DebugMenu()
{
}

void DebugMenu::StartRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = 0;
	bool show_demo_window = true;

	if (!m_editMode)
	{
		window_flags |= ImGuiWindowFlags_NoInputs;
		ImGui::SetNextWindowBgAlpha(0.35f);
	}

	ImGui::Begin("Debug Menu (I)", &show_demo_window, window_flags);
	if (!m_editMode)
		ImGui::Text("GAME MODE");
	else
		ImGui::Text("EDIT MODE");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	// Expose some other flags which are useful to showcase how they interact with Leading/Trailing tabs
	static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown;

	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		bool open = true;
		// Submit our regular tabs
		for (int n = 0; n < m_TabOptions.size(); n++)
		{
			if (ImGui::BeginTabItem(m_TabOptions[n], &open, ImGuiTabItemFlags_NoCloseWithMiddleMouseButton))
			{
				m_SelectedTab = m_TabOptions[n];
				if (m_SelectedTab == "Objects")
				{
					RenderObjectList();
				}
				else if (m_SelectedTab == "Lights")
				{
					if (ImGui::TreeNode("Ambient Light"))
					{
						SetAmbientLightMenu();
						GeneralLightMenu();
						SendAmbientLightValues();
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Directional Light"))
					{
						SetDirectionalLightMenu();
						DirectionalLightMenu();
						SendDirectionalLightValues();
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Point Lights"))
					{
						RenderPointLightList();
						ImGui::TreePop();
					}
				}
				//else if (m_SelectedTab == "ParticleSystem")
				//{
				//	ParticleSystemMenu();
				//}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void DebugMenu::NewStartRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void DebugMenu::EndRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugMenu::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}



void DebugMenu::RenderObjectList()
{
	static int item_current_idx = 0; // Here we store our selection data as an index.
	if (ImGui::BeginListBox("List of Objects"))
	{
		for (int n = 0; n < m_gameObjects.size(); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(m_gameObjects[n].GetName().c_str(), is_selected))
				item_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
	SetObjectMenu(m_gameObjects[item_current_idx]);
	TransformationMenu();

	SendObjectValues(m_gameObjects[item_current_idx]);
}

void DebugMenu::TransformationMenu()
{
	// Translation
	ImGui::Text("Translation");
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("X##Translation", &objectOptions.position.x, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Y##Translation", &objectOptions.position.y, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Z##Translation", &objectOptions.position.z, 0.1f);

	// Rotation
	ImGui::Text("Rotation");
	ImGui::DragFloat("X##Rotation", &objectOptions.rotation.x, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Y##Rotation", &objectOptions.rotation.y, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Z##Rotation", &objectOptions.rotation.z, 0.1f);

	// Scale
	ImGui::Text("Scale");
	ImGui::DragFloat("X##Scale", &objectOptions.scale.x, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Y##Scale", &objectOptions.scale.y, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Z##Scale", &objectOptions.scale.z, 0.1f);
}

void DebugMenu::SetObjectMenu(GameObject& obj)
{
	objectOptions.position = obj.GetTransform().GetPosition();
	objectOptions.rotation = obj.GetTransform().GetRotation();
	objectOptions.scale = obj.GetTransform().GetScale();
}

void DebugMenu::SendObjectValues(GameObject& obj)
{
	obj.GetTransform().SetPosition(objectOptions.position);
	obj.GetTransform().SetRotation(objectOptions.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	obj.GetTransform().SetRotation(objectOptions.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	obj.GetTransform().SetRotation(objectOptions.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	obj.GetTransform().SetScale(objectOptions.scale);
}

void DebugMenu::GeneralLightMenu()
{
	// Ambient Light
	ImGui::Text("Global Ambient Light");
	ImGui::Separator();
	ImGui::PushItemWidth(60);
	ImGui::Checkbox("Is Active", &ambientLightOptions.IsEnabled);
	ImGui::Text("Light Color");
	ImGui::DragFloat("R##Color", &ambientLightOptions.LightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::DragFloat("G##Color", &ambientLightOptions.LightColor.y, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::DragFloat("B##Color", &ambientLightOptions.LightColor.z, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::ColorPicker3("##picker", (float*)&ambientLightOptions.LightColor, ImGuiColorEditFlags_DisplayRGB);

	// Ambient Intensity
	ImGui::Text("Color Intensity");
	ImGui::DragFloat("##amb", &ambientLightOptions.ColorIntensity, 0.001f, 0.0f, 1.0f);
	ImGui::Separator();
	ImGui::Separator();
}

void DebugMenu::SetAmbientLightMenu()
{
	ambientLightOptions.ColorIntensity = m_LightSystem.GetAmbientLight().ColorIntensity;
	ambientLightOptions.LightColor = m_LightSystem.GetAmbientLight().LightColor;
	ambientLightOptions.IsEnabled = m_LightSystem.GetAmbientLight().IsEnabled;
}

void DebugMenu::SendAmbientLightValues()
{
	m_LightSystem.SetAmbientLight(ambientLightOptions);
}

void DebugMenu::SetDirectionalLightMenu()
{
	directionalLightOptions.Direction = m_LightSystem.GetDirectionalLight().Direction;
	directionalLightOptions.LightColor = m_LightSystem.GetDirectionalLight().LightColor;
	directionalLightOptions.ColorIntensity = m_LightSystem.GetDirectionalLight().ColorIntensity;
	directionalLightOptions.SpecularIntensity = m_LightSystem.GetDirectionalLight().SpecularIntensity;
	directionalLightOptions.IsEnabled = m_LightSystem.GetDirectionalLight().IsEnabled;
}

void DebugMenu::SendDirectionalLightValues()
{
	m_LightSystem.SetDirectionalLight(directionalLightOptions);
}


void DebugMenu::DirectionalLightMenu()
{
	ImGui::PushItemWidth(60);


	if (ImGuiKnobs::Knob("Light Angle", &dirAngle, 0, 360, 1, "%.0f", ImGuiKnobVariant_WiperDot))
	{
		glm::vec3 zeroAngleVector = glm::vec3(-1.0f, 0.0f, 0.0f);
		float str = 40.0f;
		float cosine = glm::cos(glm::radians(dirAngle));
		float sine = glm::sin(glm::radians(dirAngle));
		glm::vec3 rotatedVector = glm::vec3(zeroAngleVector.x * cosine + zeroAngleVector.y * sine, -zeroAngleVector.x * sine + zeroAngleVector.y * cosine, 0.0f);
		rotatedVector *= str;
		directionalLightOptions.Direction.x = -rotatedVector.x;
		directionalLightOptions.Direction.y = -rotatedVector.y;
		directionalLightOptions.Direction.z = -rotatedVector.z;
	}
	ImGui::Checkbox("Is Active", &directionalLightOptions.IsEnabled);
	ImGui::Text("Light Direction");
	ImGui::DragFloat("X##Direction", &directionalLightOptions.Direction.x, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Y##Direction", &directionalLightOptions.Direction.y, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Z##Direction", &directionalLightOptions.Direction.z, 0.1f);

	ImGui::Text("Light Color");
	ImGui::DragFloat("R##Color", &directionalLightOptions.LightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::DragFloat("G##Color", &directionalLightOptions.LightColor.y, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::DragFloat("B##Color", &directionalLightOptions.LightColor.z, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::ColorPicker3("##picker", (float*)&directionalLightOptions.LightColor, ImGuiColorEditFlags_DisplayRGB);

	ImGui::Text("Color Intensity");
	ImGui::DragFloat("##amb", &directionalLightOptions.ColorIntensity, 0.01f, 0.0f, 1.0f);

	ImGui::Text("Specular Intensity");
	ImGui::DragFloat("##spec", &directionalLightOptions.SpecularIntensity, 1.0f);
	ImGui::Separator();
	ImGui::Separator();
}

void DebugMenu::SetPointLightMenu(PointLight& light)
{
	m_SelectedPointLight = light.Name;

	pointLightOptions.Position = light.Position;
	pointLightOptions.LightColor = light.LightColor;

	pointLightOptions.radius = light.radius;
	pointLightOptions.edgeIntensity = light.edgeIntensity;
}

void DebugMenu::SendPointLightValues()
{
	m_LightSystem.SetPointLight(m_SelectedPointLight, pointLightOptions);
}

void DebugMenu::PointLightListMenu()
{
	ImGui::Text("Light Position");
	ImGui::PushItemWidth(60);
	ImGui::DragFloat("X##Position", &pointLightOptions.Position.x, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Y##Position", &pointLightOptions.Position.y, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat("Z##Position", &pointLightOptions.Position.z, 0.1f);

	ImGui::Text("Light Color");
	ImGui::DragFloat("R##Color", &pointLightOptions.LightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::DragFloat("G##Color", &pointLightOptions.LightColor.y, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::DragFloat("B##Color", &pointLightOptions.LightColor.z, 0.01f, 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::ColorPicker3("##picker", (float*)&pointLightOptions.LightColor, ImGuiColorEditFlags_DisplayRGB);

	ImGui::Text("Light Radius");
	ImGui::DragFloat("##constant", &pointLightOptions.radius, 0.1f);

	ImGui::Text("Edge Intensity");
	ImGui::DragFloat("##linear", &pointLightOptions.edgeIntensity, 0.1f, 0.0f, pointLightOptions.radius);
}

void DebugMenu::RenderPointLightList()
{
	static int item_current_idx = 0; // Here we store our selection data as an index.
	if (ImGui::BeginListBox("List of Lights"))
	{
		for (int n = 0; n < m_LightSystem.GetPointLightList().size(); n++)
		{
			const bool is_selected = (item_current_idx == n);
			if (ImGui::Selectable(m_LightSystem.GetPointLightList()[n].Name.c_str(), is_selected))
				item_current_idx = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
	SetPointLightMenu(m_LightSystem.GetPointLightList()[item_current_idx]);
	ImGui::Separator();
	ImGui::Separator();
	PointLightListMenu();
	ImGui::Separator();
	ImGui::Separator();
	SendPointLightValues();
}
