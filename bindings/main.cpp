#include "helper.h"
#include <gamelib/game.h>
#include <gamelib/shader.h>
#include "gamelib/transform.h"
#include "pyroomfactory.h"
#include "gamelib/tex.h"
#include "gamelib/spritesheet.h"
#include "gamelib/model.h"
#include "gamelib/tilegrid.h"
#include "gamelib/node.h"
#include "gamelib/text.h"
#include "gamelib/static_batch.h"
#include "gamelib/static_model.h"
#include "gamelib/shape.h"
#include "gamelib/shapes/line.h"
#include "gamelib/shapes/polyline.h"
#include "gamelib/shapes/rect.h"
#include "gamelib/playercontroller2d.h"
#include "gamelib/npccontroller2d.h"
#include "gamelib/script.h"
#include "gamelib/collisionengine.h"
#include "gamelib/follow.h"
#include "gamelib/quadstaticbatch.h"
#include "gamelib/agi/agiroom.h"
#include "gamelib/agi/agiobject.h"
#include "gamelib/agi/agiactions.h"
#include "gamelib/agi/agistrategy.h"
#include "gamelib/actions.h"
#include "gamelib/shadow.h"

namespace py = pybind11;



void exportVec(py::module_& m) {

	py::class_<glm::ivec2>(m, "IVec2")
		.def(py::init<>())
		.def(py::init<int, int>())
		.def_readwrite("x", &glm::ivec2::x)
		.def_readwrite("y", &glm::ivec2::y);

	py::class_<glm::ivec3>(m, "IVec3")
		.def(py::init<>())
		.def(py::init<int, int, int>())
		.def_readwrite("x", &glm::ivec3::x)
		.def_readwrite("y", &glm::ivec3::y)
		.def_readwrite("z", &glm::ivec3::z);

	py::class_<glm::ivec4>(m, "IVec4")
		.def(py::init<>())
		.def(py::init<int, int, int, int>())
		.def_readwrite("x", &glm::ivec4::x)
		.def_readwrite("y", &glm::ivec4::y)
		.def_readwrite("z", &glm::ivec4::z)
		.def_readwrite("w", &glm::ivec4::w);


	py::class_<glm::vec2>(m, "Vec2")
	 	.def(py::init<>())
		.def(py::init<float, float>())
		.def_readwrite("x", &glm::vec2::x)
		.def_readwrite("y", &glm::vec2::y)
		.def("__pos__", [] (const glm::vec2& v) { return v; })
		.def("__neg__", [] (const glm::vec2& v) { return -v; })
		.def("__add__", [](const glm::vec2 &a, const glm::vec2 &b){
			return a + b;
		})
		.def("__sub__", [](const glm::vec2 &a, const glm::vec2 &b){
			return a - b;
		})
		.def("__mul__", [](const glm::vec2& v, float s) {
			return v * s;
		})
		.def("__rmul__", [](const glm::vec2& v, float s) {
			return v * s;         // just reverse: s * v → v * s
		})
		.def("__repr__", [](const glm::vec2 &v) {
			return "Vec2(" + std::to_string(v.x) +
					   ", " + std::to_string(v.y) + ")";
			})
		// --- length ---
		.def("length", [](const glm::vec2 &a){
			return glm::length(a);
		})
		// --- normalize ---
		.def("normalize", [](const glm::vec2 &a){
			return glm::normalize(a);
		});

	py::class_<glm::vec3>(m, "Vec3")
			.def(py::init<>())
			.def(py::init<float, float, float>())
			.def_readwrite("x", &glm::vec3::x)
			.def_readwrite("y", &glm::vec3::y)
			.def_readwrite("z", &glm::vec3::z)
			.def("__pos__", [] (const glm::vec3& v) { return v; })
			.def("__neg__", [] (const glm::vec3& v) { return -v; })
			.def("__add__", [](const glm::vec3 &a, const glm::vec3 &b){
				return a + b;
			})
			.def("__sub__", [](const glm::vec3 &a, const glm::vec3 &b){
				return a - b;
			})
			.def("__repr__", [](const glm::vec3 &v) {
				return "Vec3(" + std::to_string(v.x) +
					   ", " + std::to_string(v.y) +
					   ", " + std::to_string(v.z) + ")";
			})
			// --- cross product ---
			.def("cross", [](const glm::vec3 &a, const glm::vec3 &b){
				return glm::cross(a, b);
			})
			// --- length ---
			.def("length", [](const glm::vec3 &a){
				return glm::length(a);
			})
			// --- normalize ---
			.def("normalize", [](const glm::vec3 &a){
				return glm::normalize(a);
			});

	py::class_<glm::vec4>(m, "Vec4")
			.def(py::init<>())
			.def(py::init<float, float, float, float>())
			.def_readwrite("r", &glm::vec4::r)
			.def_readwrite("g", &glm::vec4::g)
			.def_readwrite("b", &glm::vec4::b)
			.def_readwrite("a", &glm::vec4::a)
			.def("__repr__", [](const glm::vec4 &v) {
				return "Vec4(" + std::to_string(v.x) +
					   ", " + std::to_string(v.y) +
					   ", " + std::to_string(v.z) +
					   ", " + std::to_string(v.w) + ")";
			});

}

void exportVertices(py::module_& m) {
	py::class_<VertexColor>(m, "VertexColor")
		.def(py::init<>())
		.def(py::init<glm::vec3, glm::vec4>())
		.def_readwrite("position", &VertexColor::position)
		.def_readwrite("color", &VertexColor::color);

	py::class_<VertexColorNormal>(m, "VertexColorNormal")
		.def(py::init<>())
		.def(py::init<glm::vec3, glm::vec4, glm::vec3>())
		.def_readwrite("position", &VertexColorNormal::position)
		.def_readwrite("color", &VertexColorNormal::color);
}

PYBIND11_MODULE(gamelib, mainModule) {

	py::module_ modSkeletal = mainModule.def_submodule("skeletal", "Skeletal submodule");
	py::module_ modShapes = mainModule.def_submodule("shapes");
	py::module_ modAGI = mainModule.def_submodule("agi");

	py::enum_<agi::PriorityMode>(modAGI, "Priority")
		.value("BASIC", agi::PriorityMode::PRIORITY_BASIC)
		.value("AGI", agi::PriorityMode::PRIORITY_AGI)
		.export_values();

	py::enum_<HAlign>(mainModule, "HAlign")
		.value("LEFT", HAlign::LEFT)
		.value("CENTER", HAlign::CENTER)
		.value("RIGHT", HAlign::RIGHT)
		.export_values();

	py::enum_<Anchor>(mainModule, "Anchor")
		.value("TOP_LEFT", Anchor::TOP_LEFT)
		.value("TOP", Anchor::TOP)
		.value("TOP_RIGHT", Anchor::TOP_RIGHT)
		.value("LEFT", Anchor::LEFT)
		.value("CENTER", Anchor::CENTER)
		.value("RIGHT", Anchor::RIGHT)
		.value("BOTTOM_LEFT", Anchor::BOTTOM_LEFT)
		.value("BOTTOM", Anchor::BOTTOM)
		.value("BOTTOM_RIGHT", Anchor::BOTTOM_RIGHT)
		.export_values();

	py::class_<Shape, std::shared_ptr<Shape>>(mainModule, "Shape")
		.def("toModel", &Shape::makeModel);

	mainModule.def("newGame", []() {
		auto gameDirectory = detectGameDirectory();
		auto& g = Game::instance();
		g.init(gameDirectory);
		return &g;
	});

	exportVec(mainModule);

	exportVertices(mainModule);

	py::class_<Matrix>(mainModule, "Matrix")
		.def(py::init<>())
		// 2. Constructor from 16 numbers (list or tuple)
		.def(py::init([](py::object obj) {
			glm::mat4 m(1.0f);
			// Accept 16-element flat list/tuple
			if (py::isinstance<py::sequence>(obj)) {
				py::sequence seq = obj.cast<py::sequence>();
				if (seq.size() == 16) {
					for (int i = 0; i < 16; i++) {
						m[i / 4][i % 4] = seq[i].cast<float>();
					}
					return Matrix(m);
				}

			}
			throw std::runtime_error("Transform requires 16 numbers or a 4×4 list");
		}))
		.def("translate", &Matrix::translate, py::arg("t"))
		.def("rotate", &Matrix::rotate, py::arg("angle"), py::arg("axis"))
		.def("scale", &Matrix::scale, py::arg("s"))
		.def("rr", [] (const Matrix& t) {
			glm::mat4 inv = glm::transpose(glm::inverse(glm::mat3(t._matrix)));
			Matrix result(inv);
			return result;
		})
		.def("__mul__", [](const Matrix& t, const glm::vec3& v){ return t * v; })
		.def("__mul__", [](const Matrix& t, const glm::vec4& v){ return t * v; })
		// multiply two transforms
		.def("__mul__", [](const Matrix &a, const Matrix &b) {
			Matrix result;
			result._matrix = a._matrix * b._matrix; // glm::mat4 multiplication
			return result;
		});


	py::class_<Game>(mainModule, "Game", py::dynamic_attr())
			.def_static("instance", &Game::instance, py::return_value_policy::reference)
			.def("getShader", &Game::getShader, py::arg("id"), py::return_value_policy::reference)
			.def("init", &Game::init, py::arg("homeDir"))
			.def("room", &Game::getCurrentRoom, py::return_value_policy::reference)
			.def("setRoomFactory", &Game::setRoomFactory, py::arg("factory"), py::keep_alive<1, 2>())
			.def("run", &Game::run);

	py::class_<IBatch, std::shared_ptr<IBatch>>(mainModule, "IBatch")
		.def("getModel", &IBatch::getModel)
		.def("setLightDirection", &IBatch::setLightDirection, py::arg("direction"))
		.def("addSpriteSheet", &IBatch::addSpriteSheet, py::arg("spritesheet"));

	bindStaticBatch<VertexColorNormal, TrianglePrimitive>(mainModule, "StaticBatchColor");
	bindStaticBatch<VertexColor, LinePrimitive>(mainModule, "StaticBatchLineColor");
	bindStaticBatch<VertexTextureRepeat, QuadPrimitive>(mainModule, "StaticBatchTextNormalQuad");

	py::class_<QuadStaticBatch, StaticBatch<VertexTextureRepeat, QuadPrimitive>,
	        std::shared_ptr<QuadStaticBatch>>(mainModule, "QuadStaticBatch")
		.def(py::init<IShader*, Camera*>(), py::arg("shader"), py::arg("camera"))
		.def("addQuad", &QuadStaticBatch::addQuad,
		     py::arg("dir"), py::arg("position"), py::arg("size"),
		     py::arg("texBounds"), py::arg("repeat"));


	py::class_<IModel, std::shared_ptr<IModel>>(mainModule, "IModel")
		.def("setOnLoopEnd", [](IModel& model, py::function f) {
			model.setOnLoopEnd([f] () { f(); });
		})
		.def_property("animation", &IModel::getAnimation,
			&IModel::setAnimation);

	py::class_<StaticModel<VertexColorNormal, TrianglePrimitive>,
	        IModel, std::shared_ptr<StaticModel<VertexColorNormal, TrianglePrimitive>>>(mainModule, "TriangleColorModel")
		.def(py::init<IBatch*>(), py::arg("batch"));

	py::class_<StaticModel<VertexColor, LinePrimitive>,
			IModel, std::shared_ptr<StaticModel<VertexColor, LinePrimitive>>>(mainModule, "LineColorModel")
			.def(py::init<IBatch*>(), py::arg("batch"));


	py::class_<TileGrid, IModel, std::shared_ptr<TileGrid>>(mainModule, "TileGrid")
		.def(py::init<IBatch*, const std::string&>(), py::arg("batch"), py::arg("data"));

	py::class_<IShader>(mainModule, "Shader", py::dynamic_attr())
			.def("getBatch", &IShader::createBatch);

	py::class_<IRoomFactory, PyRoomFactory, std::shared_ptr<IRoomFactory>>(mainModule, "RoomFactory")
			.def(py::init<>())
			.def("createRoom", &IRoomFactory::createRoom);

	py::class_<Room, std::shared_ptr<Room>>(mainModule, "Room")
			.def(py::init<>())
			.def_property("root", &Room::getRootNode, nullptr)
			.def("getBatch", &Room::getBatch, py::return_value_policy::reference)
			.def("addBatch", &Room::addBatch, py::arg("key"), py::arg("batch"))
			.def("addCamera", &Room::addCamera, py::arg("key"), py::arg("camera"))
			.def("setCollisionEngine", &Room::addCollisionEngine, py::arg("engine"))
			.def("close", &Room::close)
			.def("setClearColor", [](Room &self, int r, int g, int b) {
				self.setClearColor(glm::ivec3{r, g, b});
			}, py::arg("r"), py::arg("g"), py::arg("b"));

//	py::class_<agi::AGIContext, std::shared_ptr<agi::AGIContext>>(modAGI, "AGIContext")
//		.def(py::init<const std::string&>(), py::arg("gameDir"));

	py::class_<agi::AGIRoom, Room, std::shared_ptr<agi::AGIRoom>>(modAGI, "Room")
		.def(py::init([](py::dict d) {
			agi::RoomConfig config;
			config.height = d["height"].cast<int>();
			config.priority = d["priority"].cast<std::string>();
			config.controlImage = d["bg"]["control"].cast<std::string>();
			config.bgImage = d["bg"]["picture"].cast<std::string>();
			config.priorityImage = d["bg"]["priority"].cast<std::string>();
			config.spriteSheet = d["spritesheet"].cast<std::string>();
			config.fontFile = d["fonts"].cast<std::string>();
			config.wordsFile = d["words"].cast<std::string>();
			return std::make_shared<agi::AGIRoom>(config);
		}))
		.def("addObject", &agi::AGIRoom::addObject)
		.def("rmObject", &agi::AGIRoom::rmObject, py::arg("objId"))
		.def("getObject", &agi::AGIRoom::getObject, py::arg("objId"), py::return_value_policy::reference)
		.def("print", &agi::AGIRoom::print)
		.def("showObj", &agi::AGIRoom::showObject, py::arg("view"), py::arg("msg"))
		.def("addSaid", [] (agi::AGIRoom &self, const std::vector<std::string>& words, py::function f) {
			self.addSaid(words, [f]() {
				f();
			});
		}, py::arg("phrase"), py::arg("scriptId"))
		.def_property_readonly("id", &agi::AGIRoom::getId)
		.def("play", &agi::AGIRoom::play, py::arg("script"));


	py::class_<Camera, std::shared_ptr<Camera>>(mainModule, "Camera")
			.def("setPosition",
				 [](Camera &self,
					const glm::vec3& eye,
					const glm::vec3& direction = glm::vec3(0.f, 0.f, -1.f),
					const glm::vec3& up = glm::vec3(0.f, 1.f, 0.f)) {
						self.setPosition(eye, direction, up);
					},
					py::arg("eye"), py::arg("direction") = glm::vec3(0.f, 0.f, -1.f),
					py::arg("up") = glm::vec3(0.f, 1.f, 0.f))
			.def("move", [](Camera &self, py::tuple delta) {
				self.move(glm::vec3(delta[0].cast<float>(), delta[1].cast<float>(), delta[2].cast<float>()));
			})
			.def("setBounds", &Camera::setBounds);



	py::class_<OrthoCamera, Camera, std::shared_ptr<OrthoCamera>>(mainModule, "OrthoCamera")
		.def(py::init<float, float, float, float, glm::vec4>(),
		     py::arg("width"), py::arg("height"), py::arg("near") = 0.1f, py::arg("far") = 100.f,
		     py::arg("viewport") = glm::vec4(0.f, 0.f, 0.f, 0.f))
		.def("size", [](OrthoCamera &self) {
			return py::make_tuple(self.getSize().x, self.getSize().y);
		});

	py::class_<PerspectiveCamera, Camera, std::shared_ptr<PerspectiveCamera>>(mainModule, "PerspectiveCamera")
		.def(py::init([](float fov, float near, float far, py::tuple viewport) {
			return std::make_shared<PerspectiveCamera>(glm::vec4(viewport[0].cast<float>(), viewport[1].cast<float>(),
			        viewport[2].cast<float>(), viewport[3].cast<float>()), fov, near, far);
		}), py::arg("fov") = 60.f, py::arg("near") = 0.1f, py::arg("far") = 100.f, py::arg("viewport") = py::make_tuple(0.f, 0.f, 0.f, 0.f));

	py::class_<Tex, std::shared_ptr<Tex>>(mainModule, "Tex")
		.def_static("getTexture", &Tex::getTexture, py::arg("filename"))
		.def("size", [](Tex &self) {
			return py::make_tuple(self.getSize().x, self.getSize().y);
		});

	py::class_<SpriteSheet, std::shared_ptr<SpriteSheet>>(mainModule, "SpriteSheet")
		.def_static("getSheet", &SpriteSheet::getSpriteSheet, py::arg("filename"));


	py::class_<QuadModel, IModel, std::shared_ptr<QuadModel>>(mainModule, "QuadModel");
		//.def(py::init<IBatch*, int, int, int, int>(),
		     //   py::arg("batch"), py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"));

	py::class_<QuadModelPal, IModel, std::shared_ptr<QuadModelPal>>(mainModule, "QuadModelPal");
		//.////def(py::init<IBatch*, int, int, int, int, int>(),
		//	py::arg("batchId"), py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"), py::arg("pal"));

	py::class_<QuadModelRepeat, IModel, std::shared_ptr<QuadModelRepeat>>(mainModule, "QuadModelRepeat")
		.def(py::init([](IBatch* batch, glm::ivec4 texBounds, glm::vec2 size, glm::vec2 repeat) {
			// Dummy QuadInfo for constructor
			QuadInfo info;
			info.x = 0;
			info.y = 0;
			info.width = size.x;
			info.height = size.y;
			info.tx0 = 0.f;
			info.ty0 = 0.f;
			info.tx1 = repeat.x;
			info.ty1 = repeat.y;
			info.flipx = false;
			info.flipy = false;
			info.anchorX=0;
			info.anchorY = 0;
			return std::make_shared<QuadModelRepeat>(batch, info, texBounds);
		}));
		//	py::arg("batch"), py::arg("info"), py::arg("texBounds"));

	py::class_<Component, std::shared_ptr<Component>> component(mainModule, "Component");

	py::class_<Node, std::shared_ptr<Node>>(mainModule, "Node")
		.def(py::init<>())
		.def("add", &Node::add, py::arg("child"))
		.def("addComponent", &Node::addComponent, py::arg("component"))
		.def("getPosition", &Node::getWorldPosition)
		.def("getModel", &Node::getModel, py::return_value_policy::reference)
		.def("setModel", &Node::setModel, py::arg("model"))
		.def("setPosition", &Node::setPosition)
		.def("setScale", &Node::setScale, py::arg("scale"))
		.def("setMatrix", &Node::setModelMatrix, py::arg("matrix"))
		.def("remove", &Node::remove);

	py::class_<Text, Node, std::shared_ptr<Text>>(mainModule, "Text")
		.def(py::init<IBatch*, const std::string&, const std::string&, int, HAlign, float, Anchor>(),
		     py::arg("batch"), py::arg("font"), py::arg("text"), py::arg("palette"),
		     py::arg("align") = HAlign::LEFT, py::arg("maxWidth") = 0.0f, py::arg("anchor") = Anchor::TOP_LEFT);


	py::class_<agi::AGIObject, Node, std::shared_ptr<agi::AGIObject>>(modAGI, "Object")
		.def(py::init<const std::string&, int, int>(), py::arg("id"), py::arg("x"), py::arg("y"))
		.def("reposition", &agi::AGIObject::reposition, py::arg("x"), py::arg("y"))
		.def("addBlocked", &agi::AGIObject::addBlocked, py::arg("color"))
		.def("setCallback", [](agi::AGIObject& object, int id, py::function f) {
			object.setCallback(id, [f] (agi::AGIObject* object, int x, int y) -> int {
				py::gil_scoped_acquire gil;
				return f(object, x, y).cast<int>();
			});
		})
		.def("setRectCallback", [](agi::AGIObject& object, int x0, int x1, int y0, int y1, py::function f) {
			object.setRectCallback(x0, x1, y0, y1, [f] (agi::AGIObject* object, int x, int y) -> int {
				py::gil_scoped_acquire gil;
				return f(object, x, y).cast<int>();
			});
		}, py::arg("x0"), py::arg("x1"), py::arg("y0"), py::arg("y1"), py::arg("callback"));

	py::class_<agi::AGICharacter, agi::AGIObject, std::shared_ptr<agi::AGICharacter>>(modAGI, "Character")
		.def("suspendMovement", &agi::AGICharacter::suspendMovement, py::arg("suspend"))
		.def("addToInventory", &agi::AGICharacter::addToInventory, py::arg("itemId"), py::arg("count") = 1)
		.def("has", &agi::AGICharacter::has, py::arg("itemId"));

	py::class_<agi::AGIPlayableCharacter, agi::AGICharacter, std::shared_ptr<agi::AGIPlayableCharacter>>(modAGI, "PlayableCharacter")
		.def(py::init<const std::string&, int, int, float, int>(), py::arg("id"), py::arg("x"), py::arg("y"),
			 py::arg("speed"), py::arg("direction"));

	py::class_<agi::AGINPC, agi::AGICharacter, std::shared_ptr<agi::AGINPC>>(modAGI, "NPC")
		.def(py::init<const std::string&, int, int, float, int>(), py::arg("id"), py::arg("x"), py::arg("y"), py::arg("speed"), py::arg("direction"))
		.def("setStrategy", &agi::AGINPC::setStrategy, py::arg("strategy"));

	py::class_<Controller2D, Component, std::shared_ptr<Controller2D>>(mainModule, "Controller2D");

	py::class_<Collider, Component, std::shared_ptr<Collider>>(mainModule, "Collider");

	py::class_<Shadow, Component, std::shared_ptr<Shadow>>(mainModule, "Shadow")
		.def(py::init<float, float, int>(), py::arg("angle"), py::arg("scale"), py::arg("palette"));

	py::class_<BasicCollider, Collider, std::shared_ptr<BasicCollider>>(mainModule, "BasicCollider")
		.def(py::init<std::shared_ptr<Shape>, int, int, const std::string&>(), py::arg("shape"), py::arg("flag"),
			 py::arg("mask"), py::arg("tag"));

	py::class_<PlayerController2D, Controller2D, std::shared_ptr<PlayerController2D>>(mainModule, "PlayerController2D")
		.def(py::init<float, float, int, int, float, float, float, float, glm::vec2>(), py::arg("width"), py::arg("height"),
			py::arg("maskUp"), py::arg("maskDown"), py::arg("maxSpeed"), py::arg("jumpHeight"), py::arg("timeToJumpApex"),
			py::arg("accelerationTime"), py::arg("anchor") = glm::vec2(0.f, 0.f));

	py::class_<NPCController2D, Controller2D, std::shared_ptr<NPCController2D>>(mainModule, "NPCController2D")
			.def(py::init<float, float, int, int, float, float, float, float, glm::vec2>(), py::arg("width"), py::arg("height"),
				 py::arg("maskUp"), py::arg("maskDown"), py::arg("maxSpeed"), py::arg("jumpHeight"), py::arg("timeToJumpApex"),
				 py::arg("accelerationTime"), py::arg("anchor") = glm::vec2(0.f, 0.f));

	py::class_<Follow, Component, std::shared_ptr<Follow>>(mainModule, "Follow")
		.def(py::init<const std::string&, glm::vec3>(), py::arg("camId"), py::arg("relative_pos") = glm::vec3(0,0,5));

	exportSkeletal(modSkeletal);

	py::class_<shapes::Line, Shape, std::shared_ptr<shapes::Line>>(modShapes, "Line")
		.def(py::init<glm::vec2, glm::vec2>(), py::arg("A"), py::arg("B"));

	py::class_<shapes::PolyLine, Shape, std::shared_ptr<shapes::PolyLine>>(modShapes, "PolyLine")
		.def(py::init<std::vector<glm::vec2>, bool>(), py::arg("points"), py::arg("closed"));

	py::class_<shapes::Rect, Shape, std::shared_ptr<shapes::Rect>>(modShapes, "Rect")
		.def(py::init<float, float, glm::vec2>(), py::arg("width"), py::arg("height"), py::arg("anchor") = glm::vec2(0.f, 0.f));

	py::class_<ICollisionEngine, std::shared_ptr<ICollisionEngine>>(mainModule, "ICollisionEngine");

	py::class_<SpatialHashingCollisionEngine, ICollisionEngine, std::shared_ptr<SpatialHashingCollisionEngine>>(mainModule, "CollisionEngine")
		.def(py::init<float>(), py::arg("cellSize"));


	py::class_<Action, std::shared_ptr<Action>>(mainModule, "Action");

	py::class_<CallFunc, Action, std::shared_ptr<CallFunc>>(mainModule, "CallFunc")
		.def(py::init([](py::function f) {
			return std::make_shared<CallFunc>([f]() {
				py::gil_scoped_acquire gil;
				f();
			});
		}), py::arg("func"));

	py::class_<agi::NPCStrategy, std::shared_ptr<agi::NPCStrategy>>(modAGI, "NPCStrategy");

	py::class_<agi::Wander, agi::NPCStrategy, std::shared_ptr<agi::Wander>>(modAGI, "Wander")
		.def(py::init<>());

	py::class_<Script, std::shared_ptr<Script>>(mainModule, "Script")
		.def(py::init<const std::string&>(), py::arg("id") ="")
		.def("setLoop", &Script::setLoop)
		.def("add", &Script::addAction, py::arg("id"), py::arg("action"), py::arg("after") = std::vector<int>());


	py::class_<agi::SetModel, Action, std::shared_ptr<agi::SetModel>>(modAGI, "SetModel")
		.def(py::init<const std::string&, const std::string&>(), py::arg("objectId"), py::arg("modelId"));

	py::class_<agi::Print, Action, std::shared_ptr<agi::Print>>(modAGI, "Print")
		.def(py::init<const std::string&>(), py::arg("message"));

	py::class_<agi::Animate, Action, std::shared_ptr<agi::Animate>>(modAGI, "Animate")
		.def(py::init<const std::string&, const std::string&, bool>(), py::arg("objectId"), py::arg("animationId"), py::arg("wait"));
}
