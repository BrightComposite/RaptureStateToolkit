//---------------------------------------------------------------------------

#include <iostream>
#include <chrono>
#include <random>

#include <application/Starter.h>

#include <core/function/Function.h>
#include <core/function/Thread.h>

#include <meta/XPack.h>

#include <graphics/Provider.h>

#include <freeimage/FreeImageConverter.h>
#include <freetype/FreeTypeDecoder.h>

#include <ui/Text.h>
#include <ui/UIPalette.h>
#include <ui/WidgetLayers.h>
#include <ui/Window.h>

#include <graphics/font/Font.h>

#include <windows/Message.h>

#include <vld.h>
#include <conio.h>

//---------------------------------------------------------------------------

namespace asd
{
	static Entrance open([]() {
		FreeTypeDecoder::initialize();
		FreeImageConverter::initialize();

		Color backgroundColor(0.2f, 0.2f, 0.2f);

		auto graphics = GraphicsProvider::provide();
		graphics->setClearColor(backgroundColor);

		Handle<Window> window(graphics, 0, 0, 800, 600);
		window->setBackgroundColor(backgroundColor);
		window->setCaption("asd::UI test");

		StandartUIPalette palette(window);

		auto * w = palette.create("label", window->root());

		with(ColoredButtonDecorator) {
			_.background({0.4f, 0.4f, 0.4f});
			_.pressed({0.1f, 0.1f, 0.1f});
			_.hovered({0.6f, 0.6f, 0.6f});
			_.boundary({0.5f, 0.5f, 0.5f}, 1);
			_.apply(w);
		}

		window->show();
		window->centralize();

		subscription(*window) {
			onmessage(KeyUpMessage) {
				switch(msg->key) {
					case VK_ESCAPE:
						dest.close();
						break;
				}
			};

			onmessage(WindowCloseMessage) {
				ThreadLoop::stop();
			};
		}

		ThreadLoop::add(processWindowMessage);
		ThreadLoop::add([&window]() {
			window->mouseUpdate();
		});

/*
		thread th([&window]() {
			subscription(WindowCloseMessage, *window) {
				ThreadLoop::stop();
			};
*/
		ThreadLoop::add([&window]() {
			window->update();
		});
/*
			ThreadLoop::run();
		});
*/
		ThreadLoop::run();

		return 0;
	});
}

//---------------------------------------------------------------------------
