#pragma once

#include "../../resources/ResourceManager.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace scene {

    /**
     * @brief Singleton leggero per gestire gli shader 2D comuni
     *
     * Precompila e conserva gli shader elementari (flat-color, texture,
     * texture-tinted) per garantire che ogni backend riceva lo shader
     * corretto senza duplicazioni.
     */
    class Shader2DLibrary {
    private:
        std::unordered_map<std::string, std::string> shaderSources;
        resources::ResourceManager* resourceManager = nullptr;
        bool initialized = false;

        // Singleton
        Shader2DLibrary() = default;

    public:
        static Shader2DLibrary& getInstance() {
            static Shader2DLibrary instance;
            return instance;
        }

        // Disable copy
        Shader2DLibrary(const Shader2DLibrary&) = delete;
        Shader2DLibrary& operator=(const Shader2DLibrary&) = delete;

        /**
         * @brief Inizializza la libreria con il resource manager
         */
        bool init(resources::ResourceManager& resMgr) {
            if (initialized) {
                return true;
            }

            resourceManager = &resMgr;

            // Registra gli shader built-in
            registerBuiltinShaders();

            // Carica gli shader nel resource manager
            if (!loadShadersToResourceManager()) {
                return false;
            }

            initialized = true;
            return true;
        }

        /**
         * @brief Verifica se uno shader esiste
         */
        bool hasShader(const std::string& name) const {
            return shaderSources.find(name) != shaderSources.end();
        }

        /**
         * @brief Ottieni il source code di uno shader
         */
        const std::string& getShaderSource(const std::string& name) const {
            static const std::string empty;
            auto it = shaderSources.find(name);
            return (it != shaderSources.end()) ? it->second : empty;
        }

        /**
         * @brief Registra un nuovo shader custom
         */
        void registerShader(const std::string& name, const std::string& source) {
            shaderSources[name] = source;

            // Se già inizializzato, carica subito nel resource manager
            if (initialized && resourceManager) {
                //
            }
        }

        /**
         * @brief Ottieni lista di tutti gli shader disponibili
         */
        std::vector<std::string> getAvailableShaders() const {
            std::vector<std::string> names;
            names.reserve(shaderSources.size());

            for (const auto& pair : shaderSources) {
                names.push_back(pair.first);
            }

            return names;
        }

    private:
        /**
         * @brief Registra gli shader built-in
         */
        void registerBuiltinShaders() {
            // Flat Color Shader (solid color, no texture)
            shaderSources["flat_color"] = R"(
#version 330 core

// Vertex Shader
#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;

void main() {
    v_TexCoord = a_TexCoord;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}
#endif

// Fragment Shader
#ifdef FRAGMENT_SHADER
in vec2 v_TexCoord;
out vec4 FragColor;

uniform vec4 u_Color;

void main() {
    FragColor = u_Color;
}
#endif
)";

            // Texture Shader (textured, no tinting)
            shaderSources["texture"] = R"(
#version 330 core

// Vertex Shader
#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform vec2 u_UVOffset;
uniform vec2 u_UVScale;

out vec2 v_TexCoord;

void main() {
    v_TexCoord = a_TexCoord * u_UVScale + u_UVOffset;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}
#endif

// Fragment Shader
#ifdef FRAGMENT_SHADER
in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_Texture;

void main() {
    FragColor = texture(u_Texture, v_TexCoord);
}
#endif
)";

            // Texture Tinted Shader (textured with color tinting)
            shaderSources["texture_tinted"] = R"(
#version 330 core

// Vertex Shader
#ifdef VERTEX_SHADER
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform vec2 u_UVOffset;
uniform vec2 u_UVScale;

out vec2 v_TexCoord;

void main() {
    v_TexCoord = a_TexCoord * u_UVScale + u_UVOffset;
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}
#endif

// Fragment Shader
#ifdef FRAGMENT_SHADER
in vec2 v_TexCoord;
out vec4 FragColor;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main() {
    vec4 texColor = texture(u_Texture, v_TexCoord);
    FragColor = texColor * u_Color;
}
#endif
)";
        }

        /**
         * @brief Carica gli shader nel resource manager
         */
        bool loadShadersToResourceManager() {
            if (!resourceManager) {
                return false;
            }

            // Qui dovresti utilizzare l'API del tuo ResourceManager
            // per caricare effettivamente gli shader
            //
            // Per ora assumo che tu abbia un metodo tipo:
            // resourceManager->loadShaderFromSource(name, source);
            //
            // Dato che non conosco l'API esatta, lascio questo vuoto
            // e dovrai adattarlo alla tua implementazione

            return true;
        }
    };

} // namespace scene
