// codectest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#if 0

#include <SDL.h>
#include <SDL_main.h>

#ifdef  __cplusplus
extern "C"
{
#endif

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
};
#endif

static void display_AVCodecContext(AVCodecContext *pCodecCtx)
{
	printf("pCodecCtx->bit_rate:%d \n", pCodecCtx->bit_rate);
	printf("pCodecCtx->sample_rate:%d \n", pCodecCtx->sample_rate);
	printf("pCodecCtx->channels:%d \n", pCodecCtx->channels);
	printf("pCodecCtx->frame_size:%d \n", pCodecCtx->frame_size);
	printf("pCodecCtx->frame_number:%d \n", pCodecCtx->frame_number);
	printf("pCodecCtx->delay:%d \n", pCodecCtx->delay);
	printf("pCodecCtx->frame_bits:%d \n", pCodecCtx->frame_bits);
	printf("\n");
}


int main(int argc, char *argv[])
{
	AVFormatContext    *pInFormatCtx = NULL;
	AVPacket *pPacket = NULL;
	AVFrame *pFrame = NULL;

	const char streamUrl[] = "./test.mp4";
	int *p_audioStream =NULL;
	p_audioStream = (int *)malloc(sizeof(int));
	int *p_videoStream =NULL;
	p_videoStream = (int *)malloc(sizeof(int));

	av_register_all();
	printf("av_init\n");

	pInFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pInFormatCtx, streamUrl, NULL, NULL) != 0)
	{
		printf("can't open stream\n");
		return -1;
	}

	if (avformat_find_stream_info(pInFormatCtx, NULL) < 0)
	{
		printf("couldn't find stream information\n");
		return -1;
	}
	printf("into audiostream!\r\n");

	for (int i = 0; i < pInFormatCtx->nb_streams; i++)
	{
		if (pInFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			*p_audioStream = i;
		}
		else if (pInFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			*p_videoStream = i;
		}

	}
	printf("over audiostream!\r\n");

	if (*p_audioStream == -1 && *p_videoStream == -1)
	{
		printf("error :no audo and video data\r\n");
		return -1;
	}

	if (*p_audioStream != -1)
	{
		AVCodecContext *pCodeCtx = pInFormatCtx->streams[*p_audioStream]->codec;
		AVCodec *pAudioCodec = avcodec_find_decoder(pCodeCtx->codec_id);
		if (pAudioCodec == NULL)
		{
			printf("avcode find decoder failde \n");
			return -1;
		}

		if (avcodec_open2(pCodeCtx, pAudioCodec, NULL) < 0)
		{
			printf("avcode open failed !\n");
			return -1;
		}
		display_AVCodecContext(pCodeCtx);

	}

	pPacket = av_packet_alloc();
	pFrame = av_frame_alloc();

	int cnt = 30;
	while (cnt--)
	{

		int ret = av_read_frame(pInFormatCtx, pPacket);
		if (ret < 0)
		{
			printf("av_read_frame error\n");
			break;
				
		}

		if (pPacket->stream_index == *p_videoStream)
		{
			avcodec_decode_video2()
		}



	}




	return 0;


}
#endif // 0




//***************************************************************
// @file:    test.c
// @author:  dingfang
// @date    2019-07-24 18:55:16
//***************************************************************

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
};
#endif

int openCodecContext(const AVFormatContext *pFormatCtx, int *pStreamIndex, enum AVMediaType type, AVCodecContext **ppCodecCtx)
{
	int streamIdx = -1;
	// 获取流下标
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == type)
		{
			streamIdx = i;
			break;
		}
	}
	if (streamIdx == -1)
	{
		printf("find video stream failed!\n");
		exit(-2);
	}
	// 寻找解码器
	AVCodecContext  *pCodecCtx = pFormatCtx->streams[streamIdx]->codec;
	AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (NULL == pCodec)
	{
		printf("avcode find decoder failed!\n");
		exit(-2);
	}

	//打开解码器
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("avcode open failed!\n");
		exit(-2);
	}
	*ppCodecCtx = pCodecCtx;
	*pStreamIndex = streamIdx;

	return 0;
}

int main(void)
{
	AVFormatContext    *pInFormatCtx = NULL;
	AVCodecContext  *pVideoCodecCtx = NULL;
	AVCodecContext  *pAudioCodecCtx = NULL;
	AVPacket *pPacket = NULL;
	AVFrame *pFrame = NULL;
	int ret;
	/* 支持本地文件和网络url */
	const char streamUrl[] = "./test.mp4";

	/* 1. 注册 */
	av_register_all();

	pInFormatCtx = avformat_alloc_context();

	/* 2. 打开流 */
	if (avformat_open_input(&pInFormatCtx, streamUrl, NULL, NULL) != 0)
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}

	/* 3. 获取流的信息 */
	if (avformat_find_stream_info(pInFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}

	int videoStreamIdx = -1;
	int audioStreamIdx = -1;
	/* 4. 寻找并打开解码器 */
	openCodecContext(pInFormatCtx, &videoStreamIdx, AVMEDIA_TYPE_VIDEO, &pVideoCodecCtx);
	openCodecContext(pInFormatCtx, &audioStreamIdx, AVMEDIA_TYPE_AUDIO, &pAudioCodecCtx);

	pPacket = av_packet_alloc();
	pFrame = av_frame_alloc();

	int cnt = 30;
	while (cnt--)
	{
		/* 5. 读流数据, 未解码的数据存放于pPacket */
		ret = av_read_frame(pInFormatCtx, pPacket);
		if (ret < 0)
		{
			printf("av_read_frame error\n");
			break;
		}

		/* 6. 解码, 解码后的数据存放于pFrame */
		/* 视频解码 */
		if (pPacket->stream_index == videoStreamIdx)
		{
			avcodec_decode_video2(pVideoCodecCtx, pFrame, &ret, pPacket);
			if (ret == 0)
			{
				printf("video decodec error!\n");
				continue;
			}
			printf("* * * * * * video * * * * * * * * *\n");
			printf("___height: [%d]\n", pFrame->height);
			printf("____width: [%d]\n", pFrame->width);
			printf("pict_type: [%d]\n", pFrame->pict_type);
			printf("___format: [%d]\n", pFrame->format);
			printf("* * * * * * * * * * * * * * * * * * *\n\n");
		}

		/* 音频解码 */
		if (pPacket->stream_index == audioStreamIdx)
		{
			avcodec_decode_audio4(pAudioCodecCtx, pFrame, &ret, pPacket);
			if (ret < 0)
			{
				printf("audio decodec error!\n");
				continue;
			}
			printf("* * * * * * audio * * * * * * * * * *\n");
			printf("____nb_samples: [%d]\n", pFrame->nb_samples);
			printf("__samples_rate: [%d]\n", pFrame->sample_rate);
			printf("channel_layout: [%lu]\n", pFrame->channel_layout);
			printf("________format: [%d]\n", pFrame->format);
			printf("* * * * * * * * * * * * * * * * * * *\n\n");
		}
		av_packet_unref(pPacket);
	}

	av_frame_free(&pFrame);
	av_packet_free(&pPacket);
	avcodec_close(pVideoCodecCtx);
	avcodec_close(pAudioCodecCtx);
	avformat_close_input(&pInFormatCtx);

	return 0;
}

