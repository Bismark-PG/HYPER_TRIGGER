# ⚡ HYPER TRIGGER

- **A high-speed 3D Roguelike TPS featuring a dynamic BGM system that reacts to weapon changes.**

I plan to continue updating the project to expand the depth of the Roguelike gameplay.\
Feedback is always welcome!

## 🔑 Key Features

- **Efficient Object Pooling System**
  + Implemented **Object Pooling** to manage **numerous active objects**. (e.g. : Bullets, Enemies, Particles, etc)
  + Ensures **stable framerates** by preventing performance drops caused by frequent memory allocation and deallocation.

- **Real-time Animation Debugging Tools**
  + Integrated **ImGui** to build custom tools for real-time bone control and animation blending, significantly boosting development efficiency.

- **Dynamic Audio Module (XAudio2)**
  + Developed a custom audio module using **DirectX XAudio2** and **std::chrono** for high-precision timing.
  + Accurately calculates loop points to enable seamless BGM layering that reacts instantly to gameplay changes.

## 🛠️ Development Environment

* **Language** : C/C++, HLSL
* **Library** : DirectX 11, ImGui, Assimp, DirectXTK, XAudio2
* **IDE & Tools** : Visual Studio, Adobe Photoshop
* **Target Platform:** PC (Windows)
* **Development start date** : 2025.12.11

## 🎮 How To Play

***(Only code is being distributed because the currently used sound source resources are not redistributable.)***

---

## ✨ Patch And Update History
### Prototype (2026.01.13)
- Prototype Completed!
   + Implemented core loop : Battle -> Upgrade -> Loop -> TimeOut.
 
### v1.0 (2026.02.12)
- Initial Release!
   + Fixing bugs in stage 5 and improving the ending sequence.

## 📝 Roadmap & Dev Notes
- **This section outlines known issues and future plans for the project.**

---

> ### ⁉️ Known Issues
>   + **Bug** : Buuuugs.
>
> ### 🗃️ Planned Improvements
>   + **Level Design & AI Pathfinding** : Designing complex map layouts with strategic cover and implementing the **A* (A-Star) algorithm** for enemies to intelligently navigate obstacles and pursue the player.
>   + **Game Design Depth** : Adding weapon-specific unique upgrades and expanding the card pool to deepen the Roguelike experience.
>
> ### 📬 Received Feedback
>  + ~~***(I`m Waiting!)***~~
>
> ### ✔️ Completed
>  + ***(Nothing Yet)***

---

# ©️ Credits

## ■ Texture ■

### ◆ [ChatGPT](https://chatgpt.com/)
- Texture

### ◆ [OpenGameArt](https://opengameart.org/)
- Explosion Sprite
  
### ◆ [Xbox One Controller Textures](https://forums.dolphin-emu.org/Thread-xbox-one-controller-textures?highlight=Xbox+One+Controller+Textures)
- Controller Button Textures

### ◆ Photoshop
- Textures Editing

## ■ BGM / SFX ■

### ◆ [Name](Link)
- All BGM

### ◆ [効果音ラボ (Sound Effect Lab)](https://soundeffect-lab.info/)
- SFX ***(Producers List Editing...)***

## ■ Editor Tools Site ■

### ◆ [123APPS](https://123apps.com/)
- Convert MP3 to WAV
- Edit Volume

## ■ Reference Games ■

- [Name](Link)

---

## 💻 Code Snippet

```cpp
// Code Snippet
```
