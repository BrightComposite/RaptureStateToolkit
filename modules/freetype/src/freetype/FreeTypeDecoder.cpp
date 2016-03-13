//---------------------------------------------------------------------------

#include <freetype/FreeTypeDecoder.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

//---------------------------------------------------------------------------

namespace Rapture
{
	static FT_Library ft;

	class FreeTypeFont : public Font
	{
	public:
		FreeTypeFont(const string & type, const Handle<ByteData> & raw) : FreeTypeFont(raw)
		{
			if(FT_New_Memory_Face(ft, raw->data, static_cast<FT_Long>(raw->size), 0, &face) != 0)
				throw FontDecodingException("Can't read font of type \"", type, "\"");
		}

		virtual ~FreeTypeFont()
		{
			FT_Done_Face(face);
		}

	protected:
		FreeTypeFont(const Handle<ByteData> & data) : data(data) {}

		virtual void loadSymbol(Handle<Symbol> & symbol, const Graphics * graphics, int size, wchar_t character) const override
		{
			FT_Set_Char_Size(face, 0, size << 6, 96, 96);
			//FT_Set_Pixel_Sizes(face, 0, size);

			auto glyph_index = FT_Get_Char_Index(face, character);

			if (FT_Load_Glyph(face, glyph_index, FT_LOAD_LINEAR_DESIGN) != 0)
				throw Exception("Can't make symbol: ", character);

			FT_Glyph glyph;

			if (FT_Get_Glyph(face->glyph, &glyph) != 0)
				throw Exception("Can't make glyph for symbol: ", character);

			if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, 1) != 0)
				throw Exception("Can't render glyph to bitmap!");

			FT_BitmapGlyph bitglyph = (FT_BitmapGlyph)glyph;
			FT_Bitmap & bmp = bitglyph->bitmap;

			SymbolData data;

			data.character = character;

			data.width = bmp.width;
			data.height = bmp.rows;

			data.left = bitglyph->left;
			data.top = size - bitglyph->top;

			data.advance = { glyph->advance.x >> 16, glyph->advance.y >> 16 };

			switch(bmp.pixel_mode)
			{
			case FT_PIXEL_MODE_BGRA:
				data.format = ImageFormat::bgra;
				break;

			case FT_PIXEL_MODE_GRAY:
				data.format = ImageFormat::grayscale;
				break;

			case FT_PIXEL_MODE_LCD:
				data.format = ImageFormat::rgb;
				break;

			default:
				data.format = ImageFormat::unknown;
			}

			if(data.width * data.height > 0)
			{
				data.set(bmp.buffer, bmp.width * bmp.rows);
				symbol.reinit(graphics, data);
			}
			else
				symbol.reinit(data);
		}

		virtual Handle<Symbol> & findSymbol(const Graphics * graphics, int size, wchar_t character) const override
		{
			auto & size_map = _cache[graphics];

			if(size_map == nullptr)
				size_map.init();

			auto & symbols = size_map[size];

			if(symbols == nullptr)
				symbols.init();

			return symbols[character];
		}

		Handle<ByteData> data;
		FT_Face face;
		mutable Map<const Graphics *, Map<int, Map<wchar_t, Symbol>>> _cache;
	};

	FreeTypeDecoder::FreeTypeDecoder()
	{
		if(FT_Init_FreeType(&ft))
			throw Exception("Can't intialize freetype library!");
	}

	FreeTypeDecoder::~FreeTypeDecoder()
	{
		FT_Done_FreeType(ft);
	}

	void FreeTypeDecoder::decode(Handle<Font> & output, const string & type, const Handle<ByteData> & raw)
	{
		output = handle<FreeTypeFont>(type, raw);
	}
}

//---------------------------------------------------------------------------