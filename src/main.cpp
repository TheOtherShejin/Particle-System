#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <Random/Random.h>

const int WIDTH = 800, HEIGHT = 600;
const int FPS = 60;
const std::string TITLE = "Particle System";
const int ANTIALIASING_LEVEL = 8;

const int PARTICLE_AMOUNT = 600;
const int LIFETIME = 2; // in seconds

class Particle {
public:
	sf::Vector2f position, velocity, acceleration;
	float mass, radius, lifetime;

	Particle(sf::Vector2f position, sf::Vector2f velocity, float mass, float radius, float lifetime)
		: position(position), velocity(velocity), mass(mass), radius(radius), lifetime(lifetime) {
		acceleration = sf::Vector2f(0, 0);
		Random::Init();
	}

	void Update(float dt) {
		velocity += acceleration * dt;
		position += velocity * dt;
		acceleration *= 0.0f;

		lifetime -= dt;
	}

	void ApplyForce(sf::Vector2f force) {
		acceleration += force / mass;
	}

	bool isDead() {
		return lifetime <= 0.0f;
	}

	void Draw(sf::RenderWindow& window) {
		sf::CircleShape shape(radius, 100);
		shape.setOrigin(radius, radius);
		shape.setPosition(position);
		shape.setFillColor(sf::Color::Color(255, 255 * (1 - lifetime), 0, 100*lifetime));
		window.draw(shape);
	}
};

class Emitter {
private:
	sf::VertexArray vertices;
	sf::Texture particleTexture;

	Particle GenerateParticle() {
		float mass = 10, radius = 20, lifetime = maxLifetime;
		sf::Vector2f position = this->position;
		sf::Vector2f velocity(Random::UniformDouble(-5*radius, 5*radius), Random::UniformDouble(-5*radius, 5*radius));
		return Particle(position, velocity, mass, radius, lifetime);
	}
public:
	std::vector<Particle> particles;
	sf::Vector2f position;
	int particleAmount;
	float maxLifetime;

	Emitter(sf::Vector2f position, int particleAmount, float maxLifetime) : position(position), particleAmount(particleAmount), maxLifetime(maxLifetime) {
		if (!particleTexture.loadFromFile("res/particle2.png")) {
			std::cout << "Failed to load particle texture.\n";
		}
		vertices = sf::VertexArray(sf::Quads);
	}

	void Update(float dt) {
		int particlesAddedPerFrame = std::floor(particleAmount * dt / maxLifetime);
		for (int i = 0; i < particlesAddedPerFrame; i++) {
			if (particles.size() >= particleAmount) break;
			particles.push_back(GenerateParticle());
		}

		for (auto& particle : particles) {
			particle.ApplyForce(sf::Vector2f(0, 2000));
			particle.Update(dt);
		}

		for (int i = particles.size() - 1; i >= 0; i--) {
			if (particles[i].isDead())
				particles.erase(particles.begin() + i);
		}
	}

	void Draw(sf::RenderWindow& window) {
		vertices.clear();
		for (auto& particle : particles) {
			//sf::Color color(255, 255 * (1 - particle.lifetime/maxLifetime), 0, 100 * particle.lifetime/maxLifetime);
			sf::Color color(255, 255, 255, 100*particle.lifetime/maxLifetime);
	
			vertices.append(sf::Vertex(particle.position + sf::Vector2f(-particle.radius, -particle.radius), color, {  0,  0 })); // Top Left
			vertices.append(sf::Vertex(particle.position + sf::Vector2f(-particle.radius,  particle.radius), color, {  0, 64 })); // Bottom Left
			vertices.append(sf::Vertex(particle.position + sf::Vector2f( particle.radius,  particle.radius), color, { 64, 64 })); // Bottom Right
			vertices.append(sf::Vertex(particle.position + sf::Vector2f( particle.radius, -particle.radius), color, { 64,  0 })); // Top Right
			
		}

		sf::RenderStates states;
		states.texture = &particleTexture;
		states.blendMode = sf::BlendAlpha;
		window.draw(vertices, states);
	}
};

int main() {
	sf::ContextSettings settings;
	settings.antialiasingLevel = ANTIALIASING_LEVEL;
	settings.depthBits = 0;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), TITLE, 7U, settings);
	window.setFramerateLimit(FPS);

	Emitter emitter(sf::Vector2f(400, 300), PARTICLE_AMOUNT, LIFETIME);
	float dt = 1.0 / (float)FPS;

	float mouseLastX = 0, mouseLastY = 0;
	float mouseVelX = 0, mouseVelY = 0;

	sf::Clock clock;
	int frameCount = 0;
	while (window.isOpen()) {
		float fps = 1.0/clock.restart().asSeconds();

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				window.close();
				break;
			}
		}

		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		mouseVelX = (mousePos.x - mouseLastX);
		mouseVelY = (mousePos.y - mouseLastY);
		mouseLastX = mousePos.x;
		mouseLastY = mousePos.y;

		emitter.position.x = mousePos.x;
		emitter.position.y = mousePos.y;

		emitter.Update(dt);

		if (frameCount >= 60) {
			system("cls");
			std::cout << "FPS: " << fps << '\n';
			std::cout << "Particle Count: " << emitter.particles.size() << '\n';
			std::cout << "Vertex Count: " << 4 * emitter.particles.size() << '\n';
			frameCount = 0;
		}
		frameCount++;

		//window.clear(sf::Color::Color(251, 224, 210));
		window.clear();
		emitter.Draw(window);
		window.display();
	}

	return 0;
}