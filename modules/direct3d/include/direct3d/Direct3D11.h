//---------------------------------------------------------------------------

#ifndef DIRECT3D11_H
#define DIRECT3D11_H

//---------------------------------------------------------------------------

#ifndef _WIN32
#error "You can use DirectX renderer only on Windows."
#endif

//---------------------------------------------------------------------------

#include <graphics/Graphics3D.h>

//---------------------------------------------------------------------------

#include <d3d11_2.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

#include <core/Subject.h>
#include <core/addition/Cached.h>
#include <core/container/TypedMap.h>
#include <core/addition/HandleSingleton.h>
#include <core/container/Array.h>

#include <windows/ComHandle.h>
#include <windows/ComException.h>

#include <ui/WidgetMessages.h>

//---------------------------------------------------------------------------

namespace Rapture
{
	using std::thread;

	namespace Direct3D11
	{
		class GraphicContext;
		class Graphics3D;
		class D3DModel;
		class D3DIndexedModel;
	}

	link_class(Direct3D11::GraphicContext, Class<Graphics3D>);
	link_class(Direct3D11::Graphics3D, Class<Direct3D11::GraphicContext>);
	link_class(Direct3D11::D3DModel, Class<Model>);
	link_class(Direct3D11::D3DIndexedModel, Class<Direct3D11::D3DModel>);

	namespace Direct3D11
	{
		struct GraphicsDebug
		{
			~GraphicsDebug()
			{
				if(instance != nullptr)
					instance->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			}

			ComHandle<ID3D11Debug> instance;
		};

//---------------------------------------------------------------------------

		class GraphicContext : public Rapture::Graphics3D
		{
		public:
			ComHandle<ID3D11Device2, GraphicContext> device;
			ComHandle<ID3D11DeviceContext2, GraphicContext> context;
			ComHandle<ID3D11RasterizerState, GraphicContext> solidRS;
			ComHandle<ID3D11RasterizerState, GraphicContext> wiredRS;
			ComHandle<ID3D11DepthStencilState, GraphicContext> depthState2D;
			ComHandle<ID3D11DepthStencilState, GraphicContext> depthState3D;
			ComHandle<ID3D11BlendState, GraphicContext> blendState;
			ComHandle<ID3D11SamplerState, GraphicContext> linearSampler;
			ComHandle<IDXGIFactory2, GraphicContext> dxgiFactory;

		protected:
			inline void setContext(const ComHandle<ID3D11Device1> & device, const ComHandle<ID3D11DeviceContext2> & context)
			{
				this->device = device;
				this->context = context;
			}

			inline void freeContext()
			{
				this->device = nullptr;
				this->context = nullptr;
			}

			GraphicsDebug debug;
		};

//---------------------------------------------------------------------------

		class D3DImage : public Image
		{
			friend class Graphics3D;

		public:
			D3DImage(const GraphicContext * ctx, const ImageData & data);
			virtual ~D3DImage() {}

			virtual void apply() const override;
			virtual void requestData(ImageData * output) const override;

		protected:
			ComHandle<ID3D11SamplerState> _state;
			ComHandle<ID3D11ShaderResourceView> _handle;
			const GraphicContext * _ctx;
		};

		typedef D3DImage D3DTexture;

//---------------------------------------------------------------------------

		class VertexElement : public Shared, public Precached<string, VertexElement>
		{
			friend class VertexLayout;

		public:
			string id;
			uint size;

			static VertexElement pos2;
			static VertexElement pos3;
			static VertexElement color3;
			static VertexElement color4;
			static VertexElement secondaryColor3;
			static VertexElement secondaryColor4;
			static VertexElement tex;
			static VertexElement normal;

		protected:
			VertexElement(const string & id, const char * semantic, uint index, DXGI_FORMAT format, uint size) : Precached<string, VertexElement>(id),
				id(id), semantic(semantic), index(index), format(format), size(size) {}

			const char * semantic;
			uint index;
			DXGI_FORMAT format;
		};

//---------------------------------------------------------------------------

		class VertexLayout : public Shared, public Cached<string, VertexLayout, ProtectedCache>
		{
			typedef Cached<string, VertexLayout, ProtectedCache> Cache;

		public:
			string key;
			Array<VertexElement> elements;
			uint stride;
			vector<D3D11_INPUT_ELEMENT_DESC> layout;

		protected:
			public_for_cache(Cache);

			VertexLayout(const String & key) : key(key)
			{
				stride = decodeData(key, elements, layout);
			}

			static uint decodeData(const String & data, Array<VertexElement> & elements, vector<D3D11_INPUT_ELEMENT_DESC> & layout)
			{
				uint stride = 0;

				for(const auto & str : split(data))
				{
					auto & vie = VertexElement::get(str);
					elements.push_back(vie);
					layout.push_back({vie->semantic, vie->index, vie->format, 0, stride, D3D11_INPUT_PER_VERTEX_DATA, 0});
					stride += vie->size;
				}

				return stride;
			}
		};

//---------------------------------------------------------------------------

		class VertexBuffer : public Shared
		{
		public:
			VertexBuffer(const Handle<VertexLayout> & vil, const VertexData & vd, D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			GraphicContext * ctx;
			D3D_PRIMITIVE_TOPOLOGY topology;
			Handle<VertexLayout> vil;
			ComHandle<ID3D11Buffer> handle;
			uint size;
		};

		class IndexBuffer : public Shared
		{
		public:
			IndexBuffer(const VertexIndices & indices);

			GraphicContext * ctx;
			ComHandle<ID3D11Buffer> handle;
			uint size;
		};

//---------------------------------------------------------------------------
		
		class Uniform : public Shared
		{
			friend class Graphics3D;

		public:
			Uniform(ShaderType shader, uint index, uint size);

		protected:
			void update(const void * data);

			ComHandle<ID3D11Buffer> buffer;
			uint index;
			GraphicContext * ctx;
		};

		template<class T, bool isValid = is_uniform<T>::value>
		class GenericUniform {};

		template<class T>
		class GenericUniform<T, true> : public Uniform, public StaticIdentifier<GenericUniform<T>>, public Singleton<GenericUniform<T>, ThreadLocalModel>
		{
		public:
			GenericUniform() : Uniform(T::shader, T::index, static_cast<uint>(sizeof(T))) {}

			void set(const T & data)
			{
				update(&data);
			}
		};

//---------------------------------------------------------------------------

		class FxTechnique : public Shared
		{
			friend class Graphics3D;
			friend class VertexBuffer;

			template<ShaderType>
			friend class Shader;

		public:
			FxTechnique() {}
			virtual ~FxTechnique() {}

			virtual void apply(uint pass = 0) const = 0;
			
			const Handle<VertexLayout> & layout() const
			{
				return vil;
			}

		protected:
			FxTechnique(const Handle<VertexLayout> & vil);

			GraphicContext * ctx;
			Handle<VertexLayout> vil;
			uint passes;
		};

		//---------------------------------------------------------------------------

		template<ShaderType Type>
		class Shader {};

		typedef Shader<ShaderType::Common>		CommonShader;
		typedef Shader<ShaderType::Vertex>		VertexShader;
		typedef Shader<ShaderType::Fragment>	FragmentShader;
		typedef Shader<ShaderType::Pixel>		PixelShader;

		typedef RawData<const void> ShaderCode;
		typedef Map<ShaderType, ShaderCode> ShaderCodeSet;
		typedef Map<string, ShaderCodeSet> ShaderMap;

		enum class ShaderCodeState
		{
			Raw,
			Compiled,
			Embedded
		};

		template<>
		class Shader<ShaderType::Common> : public Shared
		{
		public:
			Shader();
			virtual void apply() const = 0;

		protected:
			void read(const String & filename, ShaderCode & out);
			void compile(const String & filename, const String & entrance, const String & shaderModel, ShaderCode & out);
		
			GraphicContext * ctx;
		};

		template<>
		class Shader<ShaderType::Vertex> : public CommonShader
		{
			void init(const Handle<FxTechnique> & technique, const ShaderCode & code);

		public:
			Shader(const Handle<FxTechnique> & technique, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			Shader(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code);

			virtual void apply() const override;

			ComHandle<ID3D11VertexShader> id;
			ComHandle<ID3D11InputLayout> layout;
		};

		template<>
		class Shader<ShaderType::Pixel> : public CommonShader
		{
			void init(const Handle<FxTechnique> & technique, const ShaderCode & code);

		public:
			Shader(const Handle<FxTechnique> & technique, const String & path, ShaderCodeState state = ShaderCodeState::Compiled);
			Shader(const Handle<FxTechnique> & technique, const Handle<ShaderCode> & code);

			virtual void apply() const override;

			ComHandle<ID3D11PixelShader> id;
		};

		//---------------------------------------------------------------------------

		class Graphics3D : public GraphicContext, public Singleton<Handle<Graphics3D, Graphics3D>, ThreadLocalModel>
		{
		public:
			static inline Handle<Graphics3D, Graphics3D> & findInstance(const thread::id & id);

			static inline Graphics3D * initialize();
			static inline Graphics3D * initialize(const Handle<Graphics3D> & inst);
			static inline void free();

			virtual void clip(const IntRect & rect) override;
			virtual void rectangle(const IntRect & rect) override;
			virtual void ellipse(const IntRect & rect) override;
			virtual void rectangle(const SqRect & r) override;
			virtual void ellipse(const SqRect & r) override;

			virtual void draw(const Figure * figure, const IntRect & bounds) override;
			virtual void draw(const Figure * figure, const FloatTransform & transform) override;

			virtual void draw(const Model * model) override;

			virtual void draw(const Image * image, int x, int y) override;
			virtual void draw(const Image * image, int x, int y, int width, int height) override;
			virtual void draw(const Image * image, const IntRect & rect) override;
			virtual void draw(const Image * image, const SqRect & r) override;

			virtual void draw(const Symbol * symbol, int x, int y) override;

			virtual void present() const override;

			virtual void printInfo();
			virtual void printDebug();
			virtual void checkForErrors();

			virtual void bind(const Handle<Surface> & surface) override;
			virtual void bind(const Handle<Texture> & texture, uint index) override;

			virtual Handle<Image> createImage(const ImageData & data) const override;
			virtual Handle<Figure> createFigure(const FigureData & data) const override;

			template<class T, useif <is_uniform<T>::value> endif>
			void updateUniform(const T & uniform) const
			{
				uniformData.request<GenericUniform<T>>()->set(uniform);
			}

			static Class<Subject> meta;

			static Map<thread::id, Graphics3D, Graphics3D> pool;

		protected:
			public_for_handle(Graphics3D, Graphics3D)

			friend class D3DModel;

			Graphics3D();
			virtual ~Graphics3D();

			virtual Handle<Surface> createWindowSurface(WindowAdapter * adapter) const override;
			virtual void updateBrushState() override;

			void draw(const D3DModel * model);

			void initDevice();
			void initFacilities();
			void freeFacilities();

			static inline void updatePool();

			Handle<VertexShader> vshader = nullptr;
			Handle<PixelShader> pshader = nullptr;
			
			mutable Handle<VertexBuffer> vbuffer = nullptr;
			mutable Handle<IndexBuffer> ibuffer = nullptr;
			mutable TypedMap<Uniform> uniformData;

			Array<Texture> textures;

			D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		};

		inline Handle<Graphics3D, Graphics3D> & Graphics3D::findInstance(const thread::id & id)
		{
			return pool[id];
		}

		inline Graphics3D * Graphics3D::initialize()
		{
			auto & graphics = instance().init();

			graphics->initDevice();
			graphics->initFacilities();

			updatePool();

			return graphics;
		}

		inline Graphics3D * Graphics3D::initialize(const Handle<Graphics3D> & inst)
		{
			auto & graphics = instance();

			if(graphics == inst)
				return graphics;

			graphics = inst;
			updatePool();

			return graphics;
		}

		inline void Graphics3D::free()
		{
			auto & graphics = instance();

			if(graphics == nullptr)
				return;

			graphics->freeFacilities();
			graphics->freeContext();
			graphics = nullptr;
			updatePool();
		}

		inline void Graphics3D::updatePool()
		{
			pool[std::this_thread::get_id()] = instance();
		}

		//---------------------------------------------------------------------------

		class Shaders
		{
			friend class Graphics3D;

		public:
			static ShaderCode getCode(const string & id, ShaderType type)
			{
				auto & codeSet = shaders[id];

				if(codeSet == nullptr)
					throw Exception("Can't find embedded shader set with id \"", id, "\"");

				auto & code = (*codeSet)[type];

				if(code == nullptr)
					throw Exception("Embedded shader set with id \"", id, "\" doesn't contain shader of type \"", type, "\"");

				return *code;
			}

			static Handle<ShaderCodeSet> & setCode(const string & id, const initializer_list<ShaderCode> & codeSet)
			{
				auto & set = shaders[id].reinit();

				for(size_t i = 0; i < codeSet.size(); ++i)
					set->insert_or_assign(static_cast<ShaderType>(i), handle<ShaderCode>(*(codeSet.begin() + i)));

				return set;
			}

		protected:
			static void initialize();
			static void free();

			static ShaderMap shaders;
		};

		//---------------------------------------------------------------------------

		class D3DModel : public Model
		{
		public:
			D3DModel(const Graphics3D * graphics, const Handle<VertexLayout> & vil, const VertexData & vertexData)
				: Model(graphics, vertexData, vil->stride), buffer(vil, vertexData)
			{
				setclass(D3DModel);
			}

			Handle<VertexBuffer> buffer;

			static Handle<D3DModel, Graphics3D> quad;
			static Handle<D3DModel, Graphics3D> texquad;
		};

		class D3DIndexedModel : public D3DModel
		{
		public:
			D3DIndexedModel(const Graphics3D * graphics, const Handle<VertexLayout> & vil, const VertexData & vertexData, const VertexIndices & indices)
				: D3DModel(graphics, vil, vertexData), indices(indices)
			{
				setclass(D3DIndexedModel);
			}

			Handle<IndexBuffer> indices;
			uint indicesLocation;
		};

		class D3DFigure : public Figure
		{
		public:
			D3DFigure(const Graphics3D * graphics, const FigureData & data);

			Handle<D3DModel> model;
		};

		//---------------------------------------------------------------------------

		class D3DSurface : public Surface
		{
			friend class Graphics3D;

		public:
			D3DSurface(const IntSize & size);
			virtual ~D3DSurface();

			virtual void clear() const;

		protected:
			GraphicContext * ctx;
			void createDepthStencil();

			ComHandle<ID3D11DepthStencilView> depthStencilView;
		};

		class DepthBufferSurface : public D3DSurface
		{
		public:
			DepthBufferSurface(const IntSize & size);
			DepthBufferSurface(const DepthBufferSurface &) = delete;
			virtual ~DepthBufferSurface() {}

			virtual void apply() const override;
		};

		class WindowSurface : public D3DSurface
		{
			friend class Graphics3D;

		public:
			WindowSurface(WindowAdapter * adapter);
			WindowSurface(const WindowSurface &) = delete;
			virtual ~WindowSurface();

			virtual void apply() const override;
			virtual void present() const override;
			virtual void requestData(ImageData * data) const override;

		protected:
			void createSwapChain(bool fullscreen);
			void createRenderTarget();
			void releaseRenderTarget();

			static void findPreferredMode(const ComHandle<IDXGIOutput> & output, DXGI_MODE_DESC & mode);

			void onWindowResize(Handle<WindowResizeMessage> & msg, WindowAdapter & adapter);
			void onWindowFullscreen(Handle<WindowFullscreenMessage> & msg, WindowAdapter & adapter);
			void onWindowClose(Handle<WindowCloseMessage> & msg, WindowAdapter & adapter);

			ComHandle<IDXGISwapChain1> swapChain;
			ComHandle<ID3D11RenderTargetView> renderTargetView;

			DXGI_PRESENT_PARAMETERS presentParams;
			WindowAdapter * adapter;
		};

		class TextureSurface : public D3DSurface
		{
		public:
			TextureSurface(const IntSize & size);
			virtual ~TextureSurface() {}

			virtual void apply() const override;
		};

		//---------------------------------------------------------------------------

		class ShaderProgram : public Shared
		{
		public:
			virtual ~ShaderProgram() {}
			virtual void apply() const {}

		protected:
			ShaderProgram() {}
		};

		template<class T>
		using is_shader_program = is_base_of<ShaderProgram, T>;

		class VPShaderProgram : public ShaderProgram
		{
		public:
			VPShaderProgram(const Handle<FxTechnique> & technique, const string & filename, ShaderCodeState state = ShaderCodeState::Compiled) : vs(technique, filename, state), ps(technique, filename, state) {}
			VPShaderProgram(const Handle<FxTechnique> & technique, const Handle<ShaderCodeSet> & codeSet) : vs(technique, codeSet->at(ShaderType::Vertex)), ps(technique, codeSet->at(ShaderType::Pixel)) {}
			virtual ~VPShaderProgram() {}

			virtual void apply() const override
			{
				vs->apply();
				ps->apply();
			}

		protected:
			Handle<VertexShader> vs;
			Handle<PixelShader> ps;
		};

		//---------------------------------------------------------------------------

		class SimpleTechnique : public FxTechnique
		{
		public:
			SimpleTechnique(const Handle<VertexLayout> & vil)
				: FxTechnique(vil), program() {}

			SimpleTechnique(const Handle<VertexLayout> & vil, const Handle<ShaderProgram> & program)
				: FxTechnique(vil), program(program) {}

			template<class Program, class ... A, useif <
				is_shader_program<Program>::value,
				can_construct<Program, Handle<FxTechnique>, A...>::value
				> endif
			>
			SimpleTechnique(const Handle<VertexLayout> & vil, const Type<Program> &, A &&... args)
				: FxTechnique(vil), program(handle<Program>(this, forward<A>(args)...)) {}

			virtual ~SimpleTechnique() {}

			void bind(const Handle<ShaderProgram> & program)
			{
				this->program = program;
			}

			virtual void apply(uint pass = 0) const override
			{
				program->apply();
			}

			static Handle<SimpleTechnique, Shaders> rectangle;
			static Handle<SimpleTechnique, Shaders> ellipse;
			static Handle<SimpleTechnique, Shaders> wired_rectangle;
			static Handle<SimpleTechnique, Shaders> wired_ellipse;
			static Handle<SimpleTechnique, Shaders> figure;
			static Handle<SimpleTechnique, Shaders> image;
			static Handle<SimpleTechnique, Shaders> text;

		protected:
			Handle<ShaderProgram> program;
		};
	}

	inline void print(String & s, const DirectX::XMMATRIX & m)
	{
		s << String::assemble(
			"(",
			DirectX::XMVectorGetX(m.r[0]), ", ",
			DirectX::XMVectorGetY(m.r[0]), ", ",
			DirectX::XMVectorGetZ(m.r[0]), ", ",
			DirectX::XMVectorGetW(m.r[0]), " | ",

			DirectX::XMVectorGetX(m.r[1]), ", ",
			DirectX::XMVectorGetY(m.r[1]), ", ",
			DirectX::XMVectorGetZ(m.r[1]), ", ",
			DirectX::XMVectorGetW(m.r[1]), " | ",

			DirectX::XMVectorGetX(m.r[2]), ", ",
			DirectX::XMVectorGetY(m.r[2]), ", ",
			DirectX::XMVectorGetZ(m.r[2]), ", ",
			DirectX::XMVectorGetW(m.r[2]), " | ",

			DirectX::XMVectorGetX(m.r[3]), ", ",
			DirectX::XMVectorGetY(m.r[3]), ", ",
			DirectX::XMVectorGetZ(m.r[3]), ", ",
			DirectX::XMVectorGetW(m.r[3]), ")"
			);
	}


#ifndef Direct3D
#define Direct3D Direct3D11
	using D3DGraphics = Direct3D11::Graphics3D;
#endif
}

//---------------------------------------------------------------------------
#endif
