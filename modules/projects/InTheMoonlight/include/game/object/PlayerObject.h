//---------------------------------------------------------------------------

#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

//---------------------------------------------------------------------------

#include <physics/Physics.h>
#include <physics/PlaneObject.h>

#include <random>

//---------------------------------------------------------------------------

namespace Rapture
{
	class Contact
	{
		friend class Contacter;

	public:
		Contact(DoubleVector pos, double force) : _pos {pos}, _force(std::min(1.0, force)) {}

	protected:
		void draw(Graphics3D * graphics, const IntRect & viewport, float zoom) const
		{
			auto pos = FloatPoint {_pos.x, _pos.y} * FloatPoint {zoom, zoom};
			graphics->setColor(1.0f, 1.0f, 1.0f, float((1.0f - float(_ticks) / _maxTicks) * _force));
			graphics->ellipse(SqRect {centeredSquare(pos, zoom * float(_ticks) * float(std::min(_force, 0.5)) * 0.04f)});
		}

		DoubleVector _pos;

		double _force = 1.0f;
		int _ticks = 0;
		int _maxTicks = 100;
	};

	class Contacter : public DrawableObject
	{
	public:
		Contacter(Scene * scene) : DrawableObject(scene) {}

		void addContact(DoubleVector pos, double force)
		{
			_contacts.emplace_back(pos, force);
		}

		int contactsCount() const
		{
			return int(_contacts.size());
		}

	protected:
		virtual void update(ticks_t ticks) override
		{
			if(ticks % 4 != 0 || _contacts.size() == 0)
				return;

			auto & first = _contacts[0];

			if(first._ticks >= first._maxTicks)
				_contacts.pop_front();

			for(auto & contact : _contacts)
				contact._ticks += 1;
		}

		virtual void draw(Graphics3D * graphics, const IntRect & viewport, float zoom) const override
		{
			auto dt = hold(graphics->depthTestModeState(), false);
			auto lw = hold(graphics->lineWidthState(), 2);
			auto fm = hold(graphics->fillModeState(), FillMode::Wireframe);

			for(auto & contact : _contacts)
				contact.draw(graphics, viewport, zoom);
		}

		deque<Contact> _contacts;
	};

	declare_and_link(PlayerObject, Class<PhysicalObject, Drawable>);

	class PlayerObject : public PhysicalObject, public Drawable
	{
		friend class PlayerController;

	public:
		PlayerObject(Scene * scene, PhysicalWorld * world, const dvec & pos) :
			PhysicalObject(
				scene,
				world,
				new btBoxShape(btVector3{1.0, 1.0, 1.0}),
				pos,
				5.0
				),
			Drawable(scene),
			_contacter(scene),
			_figure(scene->graphics(), RectangleData(1.0f, 1.0f))
		{
			setclass(PlayerObject);

			_rigidBody->setLinearFactor(btVector3{1, 1, 0});
			_rigidBody->setAngularFactor(btVector3{0, 0, 1});
		}

		virtual ~PlayerObject() {}

		virtual void contactWith(PhysicalObject * obj, const ContactInfo & info) override
		{
			if(obj == nullptr || obj notkindof (PlaneObject) || info.force < 100)
				return;

			_contacter->addContact(info.pos, info.force / 5000);
		}

	protected:
		virtual void updateFoothold()
		{
			static const double stiffness = 80000.0;

			_hasFoothold = false;

			if(getLinearVelocity()[1] > 0)
				return;

			DoubleVector a;

			btVector3 from(_pos.elements);
			btVector3 to(_pos.x, _pos.y - 2.0, _pos.z);
			btCollisionWorld::AllHitsRayResultCallback res(from, to);

			_world->rayTest(from, to, res);

			for(int i = 0; i < res.m_hitPointWorld.size(); i++)
			{
				if(res.m_collisionObjects[i] != _rigidBody)
				{    
					a = res.m_hitPointWorld[i];

					if(_pos.distanceTo(a) < 1.2)
					{
						_hasFoothold = true;
						break;
					}
				}
			}
		}

		virtual void update(ticks_t ticks) override
		{
			updateFoothold();

			DoubleVector force(0.0, 0.0, 0.0);

			if(_keyMap.isPressed(VK_UP))
			{
				if(_hasFoothold)
				{
					force[1] += 2000;

					if(!_rigidBody->isActive())
						_rigidBody->activate();
				}
			}

			if(_keyMap.isPressed(VK_LEFT))
			{
				force[0] -= 200;

				if(!_rigidBody->isActive())
					_rigidBody->activate();
			}

			if(_keyMap.isPressed(VK_RIGHT))
			{
				force[0] += 200;

				if(!_rigidBody->isActive())
					_rigidBody->activate();
			}

			_rigidBody->applyCentralForce(force.elements);
		}

		virtual void draw(Graphics3D * graphics, const IntRect & viewport, float zoom) const override
		{
			graphics->setDepth(0.0f);
			auto & v = graphics->viewport();
			
			FloatTranslation t {_pos};
			FloatQuaternion r {_rot};

			graphics->setColor(_color.vector * 0.5_v);
			_figure->model.setTransform({t, r});
			_figure->draw();

			graphics->setColor(_color);
			_figure->model.setTransform({t, r, FloatScaling{0.9f}});
			_figure->draw();
		}

		colorf _color = FloatVector::one;
		Handle<Contacter> _contacter;
		Handle<Figure3D> _figure;

		bool _hasFoothold = false;

		KeyMap _keyMap;
	};

	class PlayerController : public Object
	{
	public:
		PlayerController(PlayerObject * object) : object(object)
		{
			connect(*object->scene()->space(), this, &PlayerController::onKeyDown);
			connect(*object->scene()->space(), this, &PlayerController::onKeyUp);
		}

		virtual ~PlayerController()
		{
			disconnect(*object->scene()->space(), this, &PlayerController::onKeyDown);
			disconnect(*object->scene()->space(), this, &PlayerController::onKeyUp);
		}

	protected:
		void onKeyDown(Handle<KeyDownMessage> & message, UISpace & dest)
		{
			if(message->isFirst)
				object->_keyMap.press(message->key);
		}

		void onKeyUp(Handle<KeyUpMessage> & message, UISpace & dest)
		{
			object->_keyMap.unpress(message->key);
		}

		PlayerObject * object;
	};
}

//---------------------------------------------------------------------------
#endif
