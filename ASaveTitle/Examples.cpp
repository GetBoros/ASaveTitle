#include "Examples.h"

// AsExamples
AsExamples::AsExamples()
{

}
//------------------------------------------------------------------------------------------------------------
AsExamples::AsExamples(const EShow_Preview show_preview)
{
	// 1.0 EP_Show_Path_Info Data
	std::filesystem::path path_to_display(L"C:/FileSystemTest/SubDir3/SubDirLevel2/File2.txt ");

	// 2.0. EP_Show_Func_Ptrs Data
	switch (show_preview)
	{
	case EShow_Preview::EP_None:
		break;
	case EShow_Preview::EP_Show_Path_Info:
		Display_Path_Info(path_to_display);
		break;
	case EShow_Preview::EP_Show_Byte_Info:
		Display_Byte_Info();
		break;
	case EShow_Preview::EP_Show_Void_Ptrs:
		Display_Void_Ptrs();
		break;
	case EShow_Preview::EP_Show_Func_Ptrs:
		Display_Func_Ptrs();
		break;
	case EShow_Preview::EP_Show_Func_Ptrs_Advenced:
		Display_Func_Ptrs_Advenced();
		break;
	case EShow_Preview::EP_Show_Cast_Exam:
		Display_Cast_Exam();
		break;
	case EShow_Preview::EP_Show_Html_Decode:
		Display_Html_Decoder();
		break;
	case EShow_Preview::EP_Show_Conect_To_Servers:
		Display_Connect_Server();
		break;
	case EShow_Preview::EP_Show_Std_Map_Pair_Ptrs:
		Display_Map_Pair_Ptr();
		break;
	case EShow_Preview::EP_Show_FFmpeg_Ex:
		Display_FFmpeg_Commands();
		break;
	case EShow_Preview::EP_Show_FFMpeg_Example:
		Display_FFmpeg_Examples();
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Show_Case(EShow_Preview example_preview)
{

}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Path_Info(std::filesystem::path &path)
{
	int i;
	std::wostringstream w_string_stream;  // stor folder

	i = 0;
	w_string_stream << L"Displaying path info for: " << path << std::endl;

	// 1.0. Get Parts useing *itr
	for (std::filesystem::path::iterator itr = path.begin(); itr != path.end(); ++itr)
		w_string_stream << L"path part: " << i++ << L" = " << *itr << std::endl;

	// 2.0. Format stream with filesystem help
	w_string_stream
		<< L"root_name() = " << path.root_name() << std::endl
		<< L"root_path() = " << path.root_path() << std::endl
		<< L"relative_path() = " << path.relative_path() << std::endl  // full path withour root path
		<< L"parent_path() = " << path.parent_path() << std::endl  // full path without file name
		<< L"filename() = " << path.filename() << std::endl  // file with format
		<< L"stem() = " << path.stem() << std::endl  // file format
		<< L"extension() = " << path.extension() << std::endl;  // .txt

	int yy = 0;
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Byte_Info()
{
	int number = 65535;

	const size_t type_size = sizeof(int);  // if (int = 4 byte) its (255 max DEC)
	std::bitset<8UI64> bits;  // (1 1 1 1) - 255 DEC | 1 1 1 1 || bits ||
	std::array<std::byte, type_size> bytes_arr = std::bit_cast<std::array<std::byte, type_size> >(number);

	for (std::byte byte : bytes_arr)
		bits = std::bitset<8>(std::to_integer<unsigned char>(byte) );  // 
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Void_Ptrs()
{
	double main_vector[] = { 1.0, 2.0, 3.0 };  // Main Data
	void *vector_ptr = (void *)main_vector;  // convert to void ptr
	double *ptr_listenter = (double *)vector_ptr;  // from void ptr to double
	double a = ptr_listenter[2];  // check if ok
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Func_Ptrs()
{// For what? but we can so...

	void(AsExamples:: *ptr_func_arg_0)() = 0;  // can get ptr to func with zero arguments and type void
	void (AsExamples:: *ptr_func_arg_1) (EShow_Preview) = 0;  // void - return type || ( *ptr_func_arg_1)  - name ||  (int) - func args || = 0;  = &Say_Hello; - adress func with arg int||
	AsExamples experemental;

	// 1.0 Inits
	ptr_func_arg_0 = &AsExamples::Display_Void_Ptrs;  // get func adress
	ptr_func_arg_1 = &AsExamples::Show_Case;

	// 2.0. Cast to (void *) ptr can send from empry args
	void *ptr = (void *)&ptr_func_arg_0;  // to void *
	void (AsExamples::*restored_ptr_func)() = *static_cast<void (AsExamples:: **) () >(ptr);  // from (void *)

	// 3.0. How to call
	(experemental.*restored_ptr_func)();  // Call func
	(this->*restored_ptr_func)();  // Call func
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Func_Ptrs_Advenced()
{
	void (AsExamples:: *ptr_func_update)(EShow_Preview) = 0;  // ���(void) (���, ��������� �� �������-���� ������)(���������) = ������� ���������
	void *ptr_func = 0;  // ���, ��� ��������� ������ ���� void * (������������� ���������) ����� ��������� �� ����� ��� ������
	uintptr_t func_addr = 0;  // ���, ��� ������� ��� �������� �������������� ������������� ������ ���������
	void ( *restored_func_ptr)(EShow_Preview) = 0;  // ���������� ��������� �� ������� ������� ��������� �������� ���� EShow_Preview � ���������� void
	
	// �������������� ��������� �� �������
	ptr_func_update = &AsExamples::Show_Case;  // ���������� ��������� ptr_func_update ������ �������-����� Show_Case ������ AsExamples.
	ptr_func = reinterpret_cast<void *&>(ptr_func_update);  // ���������� ���������, �� �������-���� ������ � ���� void *.
	func_addr = reinterpret_cast<uintptr_t>(ptr_func);  // �������������� void * � uintptr_t ������� �������� ����� ������� ����� ��������, ��������� ��� ��� ������������ �������������

	// �������������� ��������� �� ������� � ����� �������
	restored_func_ptr = reinterpret_cast<void (*) (EShow_Preview)> (func_addr);  // �������� func_addr � void * c arg. ����������, ����� ��������� ������� ������� ����� ���������.
	restored_func_ptr(EShow_Preview::EP_None);  //  ����� ������� ����� ��������������� ��������� � �������� ��������� ���� EShow_Preview.
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Cast_Exam()
{
	struct SBase
	{
		int A = 0;
	};
	struct SSame
	{
		int B = 0;
	};

	SBase base;
	base.A = 5;
	SSame *same = reinterpret_cast<SSame *>(&base);
	int yy = base.A;
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Html_Decoder()
{
	unsigned int unicode_char;
	std::size_t bgn, found, end;  // Begin hex, found hex end, end - index of ;
	std::string hex_code;
	std::string html_encoded;
	std::wstring decoded_str;
	std::stringstream str_strm;

	unicode_char = 0;
	bgn = 0, found = 0, end = 0;
	hex_code = {};
	html_encoded = "&#x414;&#x438;&#x441;&#x43A;&#x432;&#x430;&#x43B;&#x438;&#x444;&#x438;&#x446;&#x438;&#x440;";
	decoded_str = {};
	str_strm << std::hex;

	while ( (found = html_encoded.find("&#x", bgn) ) != std::string::npos)
	{
		decoded_str.append(html_encoded.begin() + bgn, html_encoded.begin() + found);  // �������� ����� ������ �� ���������
		end = html_encoded.find(';', found);
		if (!(end != std::string::npos) )
			break;  // ���� ��� ';', ���������� ���������

		hex_code = html_encoded.substr(found + 3, end - found - 3);  // Extract hex value
		str_strm << hex_code;  // hex to stream buffer
		str_strm >> unicode_char;  // From hex to char

		decoded_str += static_cast<wchar_t>(unicode_char);  // Add to std::wstring decoded value
		bgn = end + 1;  // offset to next value
		hex_code.clear();
		str_strm.clear();
	}
	decoded_str.append(html_encoded.begin() + bgn, html_encoded.end() );  // �������� ������� ������
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Replace_S()
{
	std::string string_need_replace("0123 I think that this is not good idea");

	string_need_replace
		.replace(0, 5, "")  // delete 0 - 5 index | from 0 to 5, change to ""
		.replace(string_need_replace.find("that"), 4, "only")  // �������� "that" �� "only" 
		.replace(string_need_replace.find("this"), 4, ""); // ������� "this"

	string_need_replace += "Well Done";
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Connect_Server()
{

}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Map_Pair_Ptr()
{
	struct S_Test { int a; int b; } S_Example = {};

	std::map<std::wstring *, S_Test *> *Base_Array = 0;
	std::pair<std::wstring *, S_Test *> pair_exmpl = {};

	// 1.0. Create pointer and pair
	Base_Array = new std::map<std::wstring *, S_Test *> {};
	pair_exmpl.first = new std::wstring(L"Hello");
	pair_exmpl.second = new S_Test {};

	// 1.1. Add to Base_Array
	Base_Array->emplace(pair_exmpl);
	
	// 1.2. How to erase from array and delete all data || Free allocated memmory
	for (std::map<std::wstring *, S_Test*>::iterator it = Base_Array->begin(); it != Base_Array->end(); )
	{
		delete it->second;
		delete it->first;

		it = Base_Array->erase(it); // ������� ������� � ���������� �������� �� ���������
	}
	delete Base_Array;
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_FFmpeg_Commands()
{//  ffmpeg -i inFile.mp3 outfile.mp3

	// Command Line Examples ffmpeg
	/*
	- Temp
	
	- Screen_Shots
		- ffmpeg -i inFile.mp4 -ss 00:01 -vframes 1 -q:v 1 -vf "scale=1920:1080" outfile.png  // Get Screen
	
	- Video:
			- Cut Video:  //  // Cut video from ss to...
				- ffmpeg -i input.mp4 -ss 00:01:18 -to 00:02:48 -c copy 0.mp4
				- ffmpeg -i input.mp4 -ss 5 -c copy output.mp4  // Before 5 second save video
				- ffmpeg -i input.mp4 -t 15 -c copy output.mp4  // Save only first 15 second

				- ffmpeg -ss 00:00:00 -to 00:00:17 -i inFile.mp4 -c copy outfile.mp4  // Save 17 second 
				- ffmpeg -i inFile.mp4 -ss 00:00:00 -to 00:00:17 out.mp4 -c copy outfile.mp4  //
				- ffmpeg -ss 14 -i inFile.mp4 -t 4 outfile.mp4  //  -ss 14 skip 14 second | -t time duration 4 seconds
				- ffmpeg -i inFile.mp4 -ss 14 -to 18 outfile.mp4

			- Convert :
		- ffmpeg -i input.mp4 -vf "scale=1920:1080, fps=60, unsharp=5:5:1.0" -c:v libx264 -b:v 14866k -profile:v high -preset slow -c:a aac -b:a 128k -ar 44100 output.mp4
		- ffmpeg -i input.mp4 -vf "scale=1920:1080,fps=60" -c:v libx264 -b:v 14866k -profile:v high -preset slow -c:a aac -b:a 128k -ar 44100 output.mp4
		- ffmpeg -i input.mp4 outfile.mp4  // Convert from avi to mp4
		- ffmpeg -i input.mp4 -q 23 quality_23.avi  // or if mp4 use -crf || -q(quality) 23 value quality low better?!?

	- Video Concatenation :
		- ffmpeg -f concat -safe 0 -i file_list.txt -c copy outfile.mp4  // Create .txt and set all files to concatenate || file 'Test_00.mp4'

	- Audio:
		- ffmpeg -i inFile.mp3 -b:a 320k outfile.mp3  // -b:a maybe bitrate for audo can change 320k to 1000k need write k

	- Video / Audio:
		- ffmpeg -i input.avi -b:v 1000k -b:a 128k outfile.mp4  // change byterate video to 1000k and audio to 128k

	- Filters: (-filter || -vf )
		- Volume:
			- ffmpeg -i inFile.mp4 -filter:a "volume=2" outfile.mp4  // -filter:a (use on audio channel) if =2 double volume
		- Mapping
			- ffmpeg -i inFile.mp4 -filter:a "channelmap=0-0|0-1" outfile.mp4  // from left 0 to right 0 and from l 0 to r 1
		- Cropping Video
			- ffmpeg -i inFile.mp4 -filter:v "crop=w=640:h=480:x=100:y=200" outfile.mp4
			- ffmpeg -i inFile.mp4 -filter:v "crop=w=2/3*in_w:h=2/3*in_h" outfile.mp4  // crpo the video to 2/3 of its origin
		- Scalling Video
			- ffmpeg -i inFile.mp4 -filter:v "scale=w=640:h=480" outfile.mp4  // Scale video to
			- ffmpeg -i inFile.mp4 -filter:v "scale=w=2/3*in_w:h=2/3*in_h" outfile.mp4  // Scale video to 2/3
			- ffmpeg -i inFile.mp4 -filter:v "scale=w=852:h=-1" outfile.mp4  // Scale video to
		- Rotation Video
			- ffmpeg -i inFile.mp4 -filter:v "rotate=45*PI/180" outfile.mp4  // Result rotate to 45 degreases
			- ffmpeg -i inFile.mp4  -vf "hflip,transpose=2,transpose=2" outfile.mp4  // Result from left to right
			- ffmpeg -i inFile.mp4  -vf "hflip" outfile.mp4  // Result from left to right
		- Equalization :
			- ffmpeg -i inFile.mp4 -filter "colorbalance=rs=-0.3:bs=0.3:rh=0.1:bh=-0.1" outfile.mp4  // r,g,b and (shadow, midtones, highlights)
			- ffmpeg -i inFile.mp4 -vf "eq=contrast=1.3" outfile.mp4  // 1.3 Darker and more 0.8 and less brighter
			- ffmpeg -i inFile.mp4 -vf "eq=brightness=0.3" outfile.mp4  // 0.3 to mush light or -0.3 too much dark
			- ffmpeg -i inFile.mp4 -vf "eq=saturation=1.5, unsharp" outfile.mp4
			- ffmpeg -i inFile.mp4 -vf "eq=gamma_r=1.5" outfile.mp4  // _r red channel _g green _b blue
		- Filter Audio:
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]showwaves=s=1280x720:mode=cline:colors=#00FF00|#FF00FF,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]showcqt=s=1280x720,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]showfreqs=s=1280x720,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]avectorscope=s=1280x720,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]avectorscope=s=1280x720:draw=line,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]showwaves=s=1920x1080:mode=cline:colors=cyan|magenta:scale=sqrt,eq=gamma=2.0[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4

		- OBS | STREAM | WRITE DESKTOP
			- ffmpeg -f gdigrab -framerate 30 -i desktop -vcodec libx264 -preset ultrafast -crf 0 -threads 0 -y output.mp4
			- ffmpeg -f gdigrab -framerate 60 -video_size 1440x900 -i desktop -vcodec libx264 -preset medium -crf 0 -threads 0 -y output_1.mp4


	*/

	// Command Line Examples yt-dlp
	/*
		- yt-dlp -g https://youtu.be/yMMYkONKCjs  // Get real & to video stream 
		- yt-dlp https://youtu.be/yMMYkONKCjs -o "output.mp4"
		- yt-dlp -o - https://youtu.be/yMMYkONKCjs | ffmpeg -i - -c copy -f mpegts udp://localhost:1234  // 


	*/
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_FFmpeg_Examples()
{
	const char *file_name = "Temp.mp4";
	unsigned int i = 0;
	unsigned int stream_num = 0;
	AVCodecID codec_type = AV_CODEC_ID_NONE;
	AVFormatContext *format_context;
	AVStream *stream = 0;

	if (!(format_context = avformat_alloc_context() ) )
		return;
	
	if (avformat_open_input(&format_context, file_name, 0, 0) != 0)
		return;

	stream_num = format_context->nb_streams;
	for (i = 0; i < stream_num; i++)
	{
		stream = format_context->streams[i];  // check all streams
		codec_type = stream->codecpar->codec_id;  // Get Codec id(name, type) ENUM

		const AVCodec *curr_codec = avcodec_find_decoder(codec_type);  //
		if (!curr_codec)
			return;
		
		if (curr_codec->type == AVMEDIA_TYPE_VIDEO)  // If type Vide do what we want || Can do the same with audio
			return;

		int yy = 0;
	}


}
//------------------------------------------------------------------------------------------------------------

// Restream || Effects ++ || yt-dlp Examples
/*
	// Get youtube url for ffmpeg
		- yt-dlp -f best -g https://www.youtube.com/watch?v=rn4PSf_-J1s
	// Get download video + audio from site useing manifest format .mpd or .m3u8
		- yt-dlp -f bestvideo+bestaudio --merge-output-format mp4 -o "4.mp4" "https://edge35-waw.live.mmcdn.com/live-edge/am"
		- yt-dlp --merge-output-format mp4 -o "4.mp4" "https://edge35-waw.live.mmcdn.com/live-edge/amlst:_haruka_-sd-0c269a"

	// Streaming
ffmpeg -i "https://manifest.googlevideo.com/api/manifest/hls_playlist/expire/1725287016/ei/CHbVZqPcM8"
 -filter_complex "[0:a]showwaves=s=1920x1080:mode=cline:colors=cyan|magenta:scale=sqrt,eq=gamma=2.0[v]" -map "[v]" -map 0:a
 -f flv "rtmp://a.rtmp.youtube.com/live2/6qr3-2umk-tquf-9kjy-563h"

 // Work GOOD
 ffmpeg -i "https://manifest.googlevideo.com/api/manifest/hls_playlist/" \
  -filter_complex "[0:a]showwaves=s=1920x360:mode=cline:colors=cyan|magenta:scale=sqrt,format=rgba,fade=t=in:st=0:d=1,alphaextract[a]; [0:v]scale=1920:1080,pad=iw:ih+360:0:0:black[v]; [a]format=rgba,colorchannelmixer=aa=0.5[overlay]; [v][overlay]overlay=0:main_h-overlay_h" \
  -c:v libx264 \
  -crf 23 -preset fast \
  -c:a aac -b:a 192k \
  -f flv "rtmp://a.rtmp.youtube.com/live2/6qr3-2umk-tquf-9kjy-563h"

 // Work
 ffmpeg -i "https://manifest.googlevideo.com/api/manifest/hls_playlist/expire/172" \
-filter_complex "[0:a]showwaves=s=1920x1080:mode=cline:colors=cyan|magenta:scale=sqrt[v];[0:v][v]hstack=inputs=2" \
-c:v libx264 
-crf 23 
-preset fast 
-c:a aac -b:a 192k 
-f flv "rtmp://a.rtmp.youtube.com/live2/6qr3-2umk-tquf-9kjy-563h"

ffmpeg -i "https://manifest.googlevideo.com/api/manifest/" \
-filter_complex "[0:a]showwaves=s=1920x360:mode=cline:colors=cyan|magenta:scale=sqrt,format=rgba,fade=t=in:st=0:d=1,alphaextract[a]; [0:v]pad=iw:ih+360:0:0:black[v]; [a]format=rgba,geq='r=0:g=0:b=0:a=1.0'[overlay]; [v][overlay]overlay=0.5:main_h-overlay_h" \
-c:v libx264 -crf 23 -preset fast -c:a aac -b:a 192k -f flv "rtmp://a.rtmp.youtube.com/live2/6qr3-2umk-tquf-9kjy-563h"

 ffmpeg -i "https://video-weaver.vie02.hls.ttvnw.net/v1/playlist/CqMGIo_mYhKfjtfZHQi4FxidzrVBb1ny2xiBiRCHDtencN6IVyhg5X0cKRqqZnE1ExwbQQZwag7E4kWh6aq_Er8EenBSpVRdfPURRluTl2z35uKdGvz85KxqBKEffD_mdXDxz4pYpj7yjcoOJolUF-5TnBL4AgrHrsN9OHjELhucsZ3WeWXaKVIFpyEP4xmGHjuOuGcoUpilRIkwz1y0VMPk4JqM3RZ5drDK9SKe0vN5of5rgXd-hYPQ4KG9E3r-sKzlw0J_czTU0sdu4i5oJUCcvTZo7c651_HVPTJRxDLPHZITmaNVqbAX0-5OAHGvzOAbE-SbcGbVmhkANxy3tvtJqd_ifGN3XSUphuhAr_I6_cP_F6Cqc9P1VAWJvnIb7djWB7QCLR4wgIG-ZTHoivwZ5Y3t6e-0YoxfnaH0AkR_bFwCa1ZLlBA3x3dQFxn0ZJ9Hi5Sc414DL2HifI8VI-a4phoEcfdvE6ljcK_mxOFL2szgRLjA8drx0PAn5LTHeebyPudAZWeLML6sOzJ4oAVAEkdTUddnzpUvQXmThbd6ctY0GCV1EORpDnJZeUjrz4XDnowftg0JbySgFeLH3wmxRW3CFEE-EacJsLPxgEV1Fw5wuLiXwI_F7PecB1RK0xCqOiUawuWxjQ9Z5UhPYzZFkY2UV4PeXKXXeGS-E0Qy2Cm7na35DD2DQBLjG_PZboPf1Q-eLmm0vCTY3Jg0Zsqozt0ogKeNbvwZ4GqmNmSXg0sqOp-YLhlTQ0Kr37ZRFB0t_EfI9-xehWGOYIF8B8OQa7tFjSnuehN5SZAY_Avi-piygj097ptj8pr-eDLgRMxk4kcNz3RvCcBKe7BkVhyXDTiAlOGwutJspVppPGbBJuraz5jZ1Z24axfluOtrgXgWgcpX0PQZYF5EYQblRFFHKOllQ4Wu8Z9WdJULZl5XFNRbWG59qjK7H4nBVETc6OspM4hKUBt5xyWo54l8W8qpgxwcEy5XRNUJ_so7fHpE9DGmOVQYsF9YNtq-f0jNh_8Jkro8fon4B0BGWQEGe6ijvJewF48Mk8hkl_1jwEybtJnOoR4aDNN6qUJwUaTpLqQaJyABKglldS13ZXN0LTIwrgo.m3u8" \
  -c:v libx264 \
  -crf 23 -preset fast \
  -c:a aac -b:a 192k \
  -f flv "rtmp://b.rtmp.youtube.com/live2?backup=1/6qr3-2umk-tquf-9kjy-563h"

  ffmpeg -i "https://video-weaver.vie02.hls.ttvnw.net/v1/playlist/CqMGIo_mYhKfjtfZHQi4FxidzrVBb1ny2xiBiRCHDtencN6IVyhg5X0cKRqqZnE1ExwbQQZwag7E4kWh6aq_Er8EenBSpVRdfPURRluTl2z35uKdGvz85KxqBKEffD_mdXDxz4pYpj7yjcoOJolUF-5TnBL4AgrHrsN9OHjELhucsZ3WeWXaKVIFpyEP4xmGHjuOuGcoUpilRIkwz1y0VMPk4JqM3RZ5drDK9SKe0vN5of5rgXd-hYPQ4KG9E3r-sKzlw0J_czTU0sdu4i5oJUCcvTZo7c651_HVPTJRxDLPHZITmaNVqbAX0-5OAHGvzOAbE-SbcGbVmhkANxy3tvtJqd_ifGN3XSUphuhAr_I6_cP_F6Cqc9P1VAWJvnIb7djWB7QCLR4wgIG-ZTHoivwZ5Y3t6e-0YoxfnaH0AkR_bFwCa1ZLlBA3x3dQFxn0ZJ9Hi5Sc414DL2HifI8VI-a4phoEcfdvE6ljcK_mxOFL2szgRLjA8drx0PAn5LTHeebyPudAZWeLML6sOzJ4oAVAEkdTUddnzpUvQXmThbd6ctY0GCV1EORpDnJZeUjrz4XDnowftg0JbySgFeLH3wmxRW3CFEE-EacJsLPxgEV1Fw5wuLiXwI_F7PecB1RK0xCqOiUawuWxjQ9Z5UhPYzZFkY2UV4PeXKXXeGS-E0Qy2Cm7na35DD2DQBLjG_PZboPf1Q-eLmm0vCTY3Jg0Zsqozt0ogKeNbvwZ4GqmNmSXg0sqOp-YLhlTQ0Kr37ZRFB0t_EfI9-xehWGOYIF8B8OQa7tFjSnuehN5SZAY_Avi-piygj097ptj8pr-eDLgRMxk4kcNz3RvCcBKe7BkVhyXDTiAlOGwutJspVppPGbBJuraz5jZ1Z24axfluOtrgXgWgcpX0PQZYF5EYQblRFFHKOllQ4Wu8Z9WdJULZl5XFNRbWG59qjK7H4nBVETc6OspM4hKUBt5xyWo54l8W8qpgxwcEy5XRNUJ_so7fHpE9DGmOVQYsF9YNtq-f0jNh_8Jkro8fon4B0BGWQEGe6ijvJewF48Mk8hkl_1jwEybtJnOoR4aDNN6qUJwUaTpLqQaJyABKglldS13ZXN0LTIwrgo.m3u8" \
  -c:v libx264   -crf 23 -preset fast   -c:a aac -b:a 192k \
  -f flv "rtmp://b.rtmp.youtube.com/live2?backup=1/6qr3-2umk-tquf-9kjy-563h"

  // Restream ��������������, ������, ������� �������
ffmpeg -i "https://video-weaver.vie02.hls.ttvnw.net/v1/playlist/CpUIEqBUXsqXxPolz7mKcvv1qyOQZjwIxZJOCoQjj4Kr0W5fMcji8ChyQ9VdPrAWrdnC9QmBjKG28XMv6pFGOEewSSGUhbAjOGQ3l6K3ZWryDH5pq_9MZQHepWA08WBNBBozWakYSy4nKJjfryly2H7qjWJ9jTjh9iGhriJiMnVXCKBOzqt5yYTLu2Im3RIATuzL3L5BrA0Wffp2sJ4wuY4nYDjN5PJDY-H1PDClx_0fiZ_7cqLctwbO1Ek9qxdqDtkpltwi2eqVbNhv4KS5fuVfwzhsa-l4PYOxjX5UdbYw5f0EGYyZm3ai4JDEb9FQxCjtPQi8ZCjkNDG2M7dUbZ6P2CbLuRxnJyWpN7K-dbPprQYx_DpiedBU__sMn5whDlzef3lgYL_0FsBOYL6hNVHWo7ChST-eLtHkdxwriuf8KSzh846xiaMa0434HQhanZx76bz4s5S6bBSe4RKnvLIKgnrWw1g5M_ZSM1wbgdGd2dhSrJQv0r5oh-tYZdHr3vOU15wN_4IcRYnVp3jR5G_RNGOz6oghSwn3sttiMtZzzDAwL9i60B56V9lUx_un4I5eOqBRicEPps1bvDpXNCZ0uBG7l6E3GS8SPyCGBlrarosuUSvgPTV6Okemm2kb-9h0GacQe2OY3k9Vh7b4NVGsd6g-8b9SkttmPgiTH8szZvCVkr6ItA4Cy8UQtNPkl1pf_fxYvPepfETEHSOsAAhS_SED9ZzaQC7TNXmS4AwdpKJhqUQsDEqgTTg-end3J__22SvSsrQ47ZMRPrgKkyC7iZZKVGqF17iZI25nueor1s6IexVW8SQ6Ri_XXws6cImpWqN0sa7hzWCRofNZuphNFbJkqlwH64phbvQrNKNZlzGqflYbTwvsBnCXAramO5qcKXUZRLsTs6w2G9bRKY1wvkJdQmU17o4JLGrGZx8Un7AN1qYuU0e-7FPglfkPeb51ZDn26uv9o9VzhAL7IzFZAUKB28GlwhwRY7dDhHgte8YkAcJnGAvTecYNSTUUCF7B-TTrDzfKThErHlWLurSs3kJIVQsaXkj9AGYlMj9jngB--7ezyf4-XRU0-u-ypd5tqJs1l4ydkn2t5T8DGe90kuJG8m1veCXsWrwI4UrIvSVdm8NnzXsm97DuamU3E9U4-7p1P6tPZRvWFWuMN1qgHyUFIDC2wFTA6h4HKH5btJ99b0dN3RI8B4pwxzlDoLN0_B4cyn5ZDJ6i-pbzXN7psZ9vM7B5BcBlzW9UkDRZD3xmFQxemdD_s02vB1oUcNssOGM8QNfgCzRbOXm8U4fg0nWrhMrwUr4tDTxWhgIOfegPu0WltAGEBG6GcL8TnbWXCYDNHLZrqzzI-Um00ihnhK4LBElPj4Wd_5m86MQIof9RsTTT1hoMS39TTbBwzr-2tAyVIAEqCWV1LXdlc3QtMjCuCg.m3u8" \
-vf "hqdn3d,scale=1920x1080,unsharp=5:5:1.0,drawtext=fontfile='C\\:/Windows/Fonts/ahronbd.ttf':text='twitch.tv/AlinaRinRin':x=w-tw-100:y=20:fontsize=24:fontcolor=green,eq=brightness=0:contrast=1" \
-b:v 6800k -c:v libx264 -crf 26 -preset fast -c:a aac -b:a 192k \
-f flv "rtmp://b.rtmp.youtube.com/live2?backup=1/6qr3-2umk-tquf-9kjy-563h"

ffmpeg -f gdigrab -framerate 30 -i desktop -vcodec libx264 -preset ultrafast -crf 0 -threads 0  -f flv "rtmp://b.rtmp.youtube.com/live2/6qr3-2umk-tquf-9kjy-563h" -an

ffmpeg -f gdigrab -framerate 30 -i desktop -vcodec libx264 -preset ultrafast -crf 0 -threads 0 -f flv "rtmp://server.address/live/6qr3-2umk-tquf-9kjy-563h"


https://www.twitch.tv/AlinaRinRin


*/