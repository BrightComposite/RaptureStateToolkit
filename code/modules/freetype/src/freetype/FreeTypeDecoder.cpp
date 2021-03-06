//---------------------------------------------------------------------------

#include <freetype/FreeTypeDecoder.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

//---------------------------------------------------------------------------

namespace asd
{
	static FT_Library ft;

	class FreeTypeFont : public Font
	{
	public:
		FreeTypeFont(const string & type, const handle<byte_data> & raw) : FreeTypeFont(raw)
		{
			if(FT_New_Memory_Face(ft, raw->ptr, static_cast<FT_Long>(raw->size), 0, &face) != 0)
				throw FontDecodingException("Can't read font of type \"", type, "\"");
		}

		virtual ~FreeTypeFont()
		{
			FT_Done_Face(face);
		}

	protected:
		FreeTypeFont(const handle<byte_data> & data) : data(data) {}

		virtual void loadSymbol(handle<Symbol> & symbol, graphics * graphics, int size, wchar_t character) const override
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

			auto bitglyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
			auto & bmp = bitglyph->bitmap;

			SymbolData data;

			data.character = character;

			data.area = { bmp.width, bmp.rows };
			
			data.left = bitglyph->left;
			data.top = size - bitglyph->top;

			data.advance = { glyph->advance.x >> 16, glyph->advance.y >> 16 };

			switch(bmp.pixel_mode)
			{
			case FT_PIXEL_MODE_BGRA:
				data.format = image_format::bgra;
				break;

			case FT_PIXEL_MODE_GRAY:
				data.format = image_format::grayscale;
				break;

			case FT_PIXEL_MODE_LCD:
				data.format = image_format::rgb;
				break;

			default:
				data.format = image_format::unknown;
			}

			if(data.width() * data.height() > 0)
			{
				data.set(bmp.buffer, bmp.pitch * bmp.rows);
				symbol.init(graphics, data);
				image::save(initial_path() / (string &&)String::assemble(char(character), ".png"), symbol->image());
			}
			else
				symbol.init(data);
		}

		virtual handle<Symbol> & findSymbol(graphics * graphics, int size, wchar_t character) const override
		{
			return _cache[graphics][size][character];
		}

		handle<byte_data> data;
		FT_Face face;
		mutable dictionary<const graphics *, dictionary<int, Map<wchar_t, Symbol>>> _cache;
	};

	FreeTypeDecoder::FreeTypeDecoder()
	{
		if(FT_Init_FreeType(&ft))
			throw Exception("Can't initialize freetype library!");
	}

	FreeTypeDecoder::~FreeTypeDecoder()
	{
		FT_Done_FreeType(ft);
	}

	void FreeTypeDecoder::decode(handle<Font> & output, const string & type, const handle<byte_data> & raw)
	{
		output = make::handle<FreeTypeFont>(type, raw);
	}
}

//---------------------------------------------------------------------------
