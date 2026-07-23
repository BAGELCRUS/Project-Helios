# Project HELIOS

> **A modular framework for the next generation of Arma Reforger Game Master tools.**

Project HELIOS is an open, modular framework designed to redefine the Game Master (GM) experience in Arma Reforger. Instead of replacing the vanilla Game Master with a single all-in-one addon, HELIOS is built as a collection of interoperable modules that can be used independently or together as a complete ecosystem.

Our mission is to provide mission creators, event staff, realism units, and server administrators with a modern toolkit for building dynamic, immersive, and scalable scenarios while maintaining performance, usability, and extensibility.

---

## Vision

Game Masters should be directors—not just object spawners.

Project HELIOS aims to provide a professional-grade framework that empowers GMs to create reactive battlefields, persistent campaigns, intelligent AI encounters, and streamlined administrative workflows.

Whether you're hosting a small co-op session or a 128-player operation, HELIOS is designed to give you the tools to adapt the battlefield in real time.

---

# Core Principles

- **Modular** – Install only the systems you need.
- **Performance First** – Built with multiplayer scalability in mind.
- **Framework Driven** – Shared systems reduce duplication and simplify development.
- **Extensible** – Public APIs allow other mods to integrate with HELIOS.
- **Community Focused** – Open development with community contributions encouraged.

---

# Project Modules

## ODIN Core

The foundation of the HELIOS ecosystem.

Provides:

- Module management
- Shared APIs
- Permissions
- Configuration framework
- Event system
- Scheduler
- Logging
- UI framework
- Common utilities

All HELIOS modules depend on ODIN Core.

---

## ATHENA

Gamemaster Improvements.

Planned features:

**Entity Utilities**
- Toggle entity visibility
- Enable/disable entity damage
- Modify entity and player faction affiliation

**Character & Group Control**
- Change AI formations
- Set movement speed
- Adjust stance
- Advanced group command tools

**Vehicle Management**
- Lock/unlock vehicles
- Toggle engine state
- Control vehicle lighting
- Modify individual vehicle component health

**Game Master Modules**
- Sound module
- Grass cutter
- Prefab deletion tools

**Game Master Systems**
- Hear local VON while in GM mode
- Dedicated GM voice communication
- Toggle Night Vision/Thermal/Lamp camera modes
- Enhanced Game Master quality-of-life features

---

## ARES

AI Systems Overhaul.

Planned features:

- cedfefef

---

## HEPHAESTUS

Loadouts and inventory editor.

Planned features:

**Advanced Loadout Editor**
- Modern replacement for BACON
- Full support for modded equipment
- Save, load, and share custom loadouts
- Faction- and role-specific loadout templates
- Search, filtering, and categorization of equipment

**Inventory Management**
- Edit player inventories directly from Game Master
- Modify vehicle cargo and storage
- Manage container and crate inventories
- Edit inventories of supported world entities
- Drag-and-drop inventory interface

**Administrative Tools**
- Apply loadouts to individuals or groups
- Copy and paste inventories between entities
- Export and import loadout presets
- Validate loadouts against available equipment
- Support for custom mission and community templates

---

## 🗺 ATLAS

Map Improvements.

Planned features:

- Ability to toggle the "you are near" tool tip in the map
- Marker text stays visible regardless of zoom level
- Contours thickness and color changed for better visibility
- Major and minor hill values adjusted for better elevation markings

---

## APOLLO

Medical systems.

Planned features:

- effefefe

---

## IRIS

Interface Changes.

Planned features:

- Mod currently contains a method to toggle various notification types on and off for the client

---

# Architecture

```
Project HELIOS
├── ODIN Core
├── ATHENA
├── ARES
├── HEPHAESTUS
├── IRIS
├── ATLAS
└── Apollo
```

---

# Development Philosophy

Project HELIOS follows a **framework-first** design philosophy.

Shared functionality belongs in **ODIN Core**, while specialized functionality is developed as independent modules. This approach minimizes duplicated code, promotes consistency, and allows communities to tailor their installation to their specific needs.

The project is designed with long-term maintainability and expansion in mind, allowing new modules to be introduced without disrupting existing functionality.

---

# Roadmap

### Phase 1
- ODIN Core
- Framework APIs
- Module loader
- Shared settings
- Permission system

---

# Contributing

Community contributions are welcome.

If you'd like to contribute:

- Open an issue before beginning major work.
- Follow the project's coding standards.
- Keep pull requests focused and well documented.
- Include documentation for new features whenever possible.

---

# Status

🚧 **Early Development**

Project HELIOS is actively under development. Interfaces, APIs, and module functionality may change prior to the first stable release.

---

# License

This project is released under the MIT License. See the `LICENSE` file for details.
