#include <SFML/Graphics.hpp>

class empty_scene_exception_t{};
// Define your scene_t class
class scene_t {
    private:
    int current_shape_index=0;
public:
    std::vector<sf::Shape*> shapes; // Mutable list of SFML shapes

    // Add a shape to the scene
    void addShape(sf::Shape* shape) {
        shapes.push_back(shape);
    }

    sf::Shape* get_selected() {
        if(shapes.size() > 0) {
            return shapes[current_shape_index];
        } else {
            throw empty_scene_exception_t();
        }
    }
    void select_next() {
        current_shape_index++;
        if(current_shape_index>=shapes.size()) {
            current_shape_index=0;
        }
    }
    void select_prev() {
        current_shape_index--;
        if(current_shape_index<0) {
            current_shape_index=shapes.size()-1;
        }
    }
};


int main() {
    sf::RenderWindow window(sf::VideoMode(640, 480), "SFML Example");

    // Create scene
    scene_t scene;

    // Create some shapes and add them to the scene
    sf::RectangleShape rectangle(sf::Vector2f(200, 150));
    rectangle.setPosition(100, 100);
    rectangle.setFillColor(sf::Color::Blue);
    scene.addShape(&rectangle);

    sf::CircleShape circle(50);
    circle.setPosition(400, 200);
    circle.setFillColor(sf::Color::Green);
    scene.addShape(&circle);
    sf::Shape *current=&circle;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle keyboard events
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }

            // Handle mouse events
            if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::KeyPressed) {
                const sf::Vector2f pos = scene.get_selected()->getPosition();


                switch (event.mouseButton.button){
                    case sf::Keyboard::Num1 :
                        scene.select_prev();
                        // Left mouse button pressed
                        // You can get the mouse position using event.mouseButton.x and event.mouseButton.y
                    break;
                    case sf::Keyboard::Num2 :
                        scene.select_next();
                        // Left mouse button pressed
                        // You can get the mouse position using event.mouseButton.x and event.mouseButton.y
                    break;
                    case sf::Keyboard::Left :
                        scene.get_selected()->setPosition(pos.x-10,pos.y);
                        // Left mouse button pressed
                        // You can get the mouse position using event.mouseButton.x and event.mouseButton.y
                    break;
                    case sf::Keyboard::Right :
                        scene.get_selected()->setPosition(pos.x+10,pos.y);
                        // Left mouse button pressed
                        // You can get the mouse position using event.mouseButton.x and event.mouseButton.y
                    break;
                    case sf::Keyboard::Up :
                        scene.get_selected()->setPosition(pos.x,pos.y-10);
                        // Left mouse button pressed
                        // You can get the mouse position using event.mouseButton.x and event.mouseButton.y
                    break;
                    case sf::Keyboard::Down :
                        scene.get_selected()->setPosition(pos.x,pos.y+10);
                        // Left mouse button pressed
                        // You can get the mouse position using event.mouseButton.x and event.mouseButton.y
                    break;
                    case sf::Mouse::Left :
                        // Left mouse button pressed
                        // You can get the mouse position using event.mouseButton.x and event.mouseButton.y
                    break;
                    case sf::Mouse::Right :
                        // Left mouse button pressed
                        // You can get the mouse position using event.mouseButton.x and event.mouseButton.y
                    break;
                }
            }
        }

        window.clear(sf::Color::White);

        
        // Draw shapes from the scene
        for (const auto& shape : scene.shapes) {
            window.draw(*shape);
        }

        // More drawing...

        window.display();
    }

    return 0;
}
