#ifndef _SampleGesturesIncluded_
#define _SampleGesturesIncluded_

#include "GeometricRecognizerTypes.h"

// TODO: read from a file. This approach is just ugly
namespace DollarRecognizer
{
	//--- Make it a struct so that everything defaults to public
	struct SampleGestures
	{
		Path2D getGestureCircle()
		{
			Path2D path;
			path.push_back(Point2D(127,141));
			path.push_back(Point2D(124,140));
			path.push_back(Point2D(120,139));
			path.push_back(Point2D(118,139));
			path.push_back(Point2D(116,139));
			path.push_back(Point2D(111,140));
			path.push_back(Point2D(109,141));
			path.push_back(Point2D(104,144));
			path.push_back(Point2D(100,147));
			path.push_back(Point2D(96,152));
			path.push_back(Point2D(93,157));
			path.push_back(Point2D(90,163));
			path.push_back(Point2D(87,169));
			path.push_back(Point2D(85,175));
			path.push_back(Point2D(83,181));
			path.push_back(Point2D(82,190));
			path.push_back(Point2D(82,195));
			path.push_back(Point2D(83,200));
			path.push_back(Point2D(84,205));
			path.push_back(Point2D(88,213));
			path.push_back(Point2D(91,216));
			path.push_back(Point2D(96,219));
			path.push_back(Point2D(103,222));
			path.push_back(Point2D(108,224));
			path.push_back(Point2D(111,224));
			path.push_back(Point2D(120,224));
			path.push_back(Point2D(133,223));
			path.push_back(Point2D(142,222));
			path.push_back(Point2D(152,218));
			path.push_back(Point2D(160,214));
			path.push_back(Point2D(167,210));
			path.push_back(Point2D(173,204));
			path.push_back(Point2D(178,198));
			path.push_back(Point2D(179,196));
			path.push_back(Point2D(182,188));
			path.push_back(Point2D(182,177));
			path.push_back(Point2D(178,167));
			path.push_back(Point2D(170,150));
			path.push_back(Point2D(163,138));
			path.push_back(Point2D(152,130));
			path.push_back(Point2D(143,129));
			path.push_back(Point2D(140,131));
			path.push_back(Point2D(129,136));
			path.push_back(Point2D(126,139));

			return path;
		}

		Path2D getGestureLeftToRightLine()
		{
			Path2D path;
			path.push_back(Point2D(80,160));
			path.push_back(Point2D(82,160));
			path.push_back(Point2D(82,160));
			path.push_back(Point2D(83,160));
			path.push_back(Point2D(83,160));
			path.push_back(Point2D(85,160));
			path.push_back(Point2D(88,160));
			path.push_back(Point2D(90,160));
			path.push_back(Point2D(92,160));
			path.push_back(Point2D(94,160));
			path.push_back(Point2D(99,160));
			path.push_back(Point2D(102,160));
			path.push_back(Point2D(106,160));
			path.push_back(Point2D(109,160));
			path.push_back(Point2D(117,160));
			path.push_back(Point2D(123,160));
			path.push_back(Point2D(126,160));
			path.push_back(Point2D(135,160));
			path.push_back(Point2D(142,160));
			path.push_back(Point2D(145,160));
			path.push_back(Point2D(152,160));
			path.push_back(Point2D(154,160));
			path.push_back(Point2D(165,160));
			path.push_back(Point2D(174,160));
			path.push_back(Point2D(179,160));
			path.push_back(Point2D(186,160));
			path.push_back(Point2D(191,160));
			path.push_back(Point2D(195,160));
			path.push_back(Point2D(197,160));
			path.push_back(Point2D(201,160));
			path.push_back(Point2D(202,160));

			return path;
		}

		Path2D getGestureLineDownDiagonal()
		{
			Path2D path;
			path.push_back(Point2D(45,215));
			path.push_back(Point2D(45,215));
			path.push_back(Point2D(45,215));
			path.push_back(Point2D(45,215));
			path.push_back(Point2D(45,215));
			path.push_back(Point2D(45,215));
			path.push_back(Point2D(45,215));
			path.push_back(Point2D(47,217));
			path.push_back(Point2D(49,220));
			path.push_back(Point2D(82,249));
			path.push_back(Point2D(159,292));
			path.push_back(Point2D(207,318));
			path.push_back(Point2D(298,374));
			path.push_back(Point2D(346,397));
			path.push_back(Point2D(385,416));
			path.push_back(Point2D(427,445));
			path.push_back(Point2D(443,454));
			path.push_back(Point2D(446,457));
			path.push_back(Point2D(447,457));
			path.push_back(Point2D(447,457));
			path.push_back(Point2D(447,457));
			path.push_back(Point2D(450,459));
			path.push_back(Point2D(456,465));
			path.push_back(Point2D(461,469));
			path.push_back(Point2D(464,473));
			path.push_back(Point2D(464,473));
			path.push_back(Point2D(464,473));
			path.push_back(Point2D(464,473));

			return path;
		}

		Path2D getGestureRectangle()
		{
			Path2D path;
			path.push_back(Point2D(78,149));
			path.push_back(Point2D(78,153));
			path.push_back(Point2D(78,157));
			path.push_back(Point2D(78,160));
			path.push_back(Point2D(79,162));
			path.push_back(Point2D(79,164));
			path.push_back(Point2D(79,167));
			path.push_back(Point2D(79,169));
			path.push_back(Point2D(79,173));
			path.push_back(Point2D(79,178));
			path.push_back(Point2D(79,183));
			path.push_back(Point2D(80,189));
			path.push_back(Point2D(80,193));
			path.push_back(Point2D(80,198));
			path.push_back(Point2D(80,202));
			path.push_back(Point2D(81,208));
			path.push_back(Point2D(81,210));
			path.push_back(Point2D(81,216));
			path.push_back(Point2D(82,222));
			path.push_back(Point2D(82,224));
			path.push_back(Point2D(82,227));
			path.push_back(Point2D(83,229));
			path.push_back(Point2D(83,231));
			path.push_back(Point2D(85,230));
			path.push_back(Point2D(88,232));
			path.push_back(Point2D(90,233));
			path.push_back(Point2D(92,232));
			path.push_back(Point2D(94,233));
			path.push_back(Point2D(99,232));
			path.push_back(Point2D(102,233));
			path.push_back(Point2D(106,233));
			path.push_back(Point2D(109,234));
			path.push_back(Point2D(117,235));
			path.push_back(Point2D(123,236));
			path.push_back(Point2D(126,236));
			path.push_back(Point2D(135,237));
			path.push_back(Point2D(142,238));
			path.push_back(Point2D(145,238));
			path.push_back(Point2D(152,238));
			path.push_back(Point2D(154,239));
			path.push_back(Point2D(165,238));
			path.push_back(Point2D(174,237));
			path.push_back(Point2D(179,236));
			path.push_back(Point2D(186,235));
			path.push_back(Point2D(191,235));
			path.push_back(Point2D(195,233));
			path.push_back(Point2D(197,233));
			path.push_back(Point2D(200,233));
			path.push_back(Point2D(201,235));
			path.push_back(Point2D(201,233));
			path.push_back(Point2D(199,231));
			path.push_back(Point2D(198,226));
			path.push_back(Point2D(198,220));
			path.push_back(Point2D(196,207));
			path.push_back(Point2D(195,195));
			path.push_back(Point2D(195,181));
			path.push_back(Point2D(195,173));
			path.push_back(Point2D(195,163));
			path.push_back(Point2D(194,155));
			path.push_back(Point2D(192,145));
			path.push_back(Point2D(192,143));
			path.push_back(Point2D(192,138));
			path.push_back(Point2D(191,135));
			path.push_back(Point2D(191,133));
			path.push_back(Point2D(191,130));
			path.push_back(Point2D(190,128));
			path.push_back(Point2D(188,129));
			path.push_back(Point2D(186,129));
			path.push_back(Point2D(181,132));
			path.push_back(Point2D(173,131));
			path.push_back(Point2D(162,131));
			path.push_back(Point2D(151,132));
			path.push_back(Point2D(149,132));
			path.push_back(Point2D(138,132));
			path.push_back(Point2D(136,132));
			path.push_back(Point2D(122,131));
			path.push_back(Point2D(120,131));
			path.push_back(Point2D(109,130));
			path.push_back(Point2D(107,130));
			path.push_back(Point2D(90,132));
			path.push_back(Point2D(81,133));
			path.push_back(Point2D(76,133));

			return path;
		}

		Path2D getGestureRightToLeftLine()
		{
			Path2D path;
			path.push_back(Point2D(200,160));
			path.push_back(Point2D(195,160));
			path.push_back(Point2D(190,160));
			path.push_back(Point2D(185,160));
			path.push_back(Point2D(180,160));
			path.push_back(Point2D(175,160));
			path.push_back(Point2D(170,160));
			path.push_back(Point2D(165,160));
			path.push_back(Point2D(160,160));
			path.push_back(Point2D(155,160));
			path.push_back(Point2D(150,160));
			path.push_back(Point2D(160,160));
			path.push_back(Point2D(140,160));
			path.push_back(Point2D(160,160));
			path.push_back(Point2D(155,160));
			path.push_back(Point2D(150,160));
			path.push_back(Point2D(145,160));
			path.push_back(Point2D(140,160));
			path.push_back(Point2D(135,160));
			path.push_back(Point2D(130,160));
			path.push_back(Point2D(125,160));
			path.push_back(Point2D(120,160));
			path.push_back(Point2D(115,160));
			path.push_back(Point2D(110,160));
			path.push_back(Point2D(105,160));
			path.push_back(Point2D(100,160));
			path.push_back(Point2D(95,160));
			path.push_back(Point2D(85,160));
			path.push_back(Point2D(75,160));
			path.push_back(Point2D(70,160));

			return path;
		}

		Path2D getGestureRightToLeftLine2()
		{
			Path2D path;
			path.push_back(Point2D(200,160));
			path.push_back(Point2D(195,170));
			path.push_back(Point2D(190,160));
			path.push_back(Point2D(185,170));
			path.push_back(Point2D(180,160));
			path.push_back(Point2D(175,170));
			path.push_back(Point2D(170,170));
			path.push_back(Point2D(165,160));
			path.push_back(Point2D(160,170));
			path.push_back(Point2D(155,160));
			path.push_back(Point2D(150,170));
			path.push_back(Point2D(160,160));
			path.push_back(Point2D(140,170));
			path.push_back(Point2D(160,160));
			path.push_back(Point2D(155,170));
			path.push_back(Point2D(150,160));
			path.push_back(Point2D(145,170));
			path.push_back(Point2D(140,160));
			path.push_back(Point2D(135,170));
			path.push_back(Point2D(130,160));
			path.push_back(Point2D(125,170));
			path.push_back(Point2D(120,160));
			path.push_back(Point2D(115,170));
			path.push_back(Point2D(110,160));
			path.push_back(Point2D(105,170));
			path.push_back(Point2D(100,160));
			path.push_back(Point2D(95,170));
			path.push_back(Point2D(85,170));
			path.push_back(Point2D(75,160));
			path.push_back(Point2D(70,170));

			return path;
		}

		Path2D getGestureRightToLeftSlashDown()
		{
			Path2D path;
			path.push_back(Point2D(200,170));
			path.push_back(Point2D(195,171));
			path.push_back(Point2D(190,172));
			path.push_back(Point2D(185,173));
			path.push_back(Point2D(180,174));
			path.push_back(Point2D(175,175));
			path.push_back(Point2D(170,176));
			path.push_back(Point2D(165,177));
			path.push_back(Point2D(160,178));
			path.push_back(Point2D(155,179));
			path.push_back(Point2D(150,180));
			path.push_back(Point2D(160,181));
			path.push_back(Point2D(140,182));
			path.push_back(Point2D(160,183));
			path.push_back(Point2D(155,184));
			path.push_back(Point2D(150,185));
			path.push_back(Point2D(145,186));
			path.push_back(Point2D(140,187));
			path.push_back(Point2D(135,188));
			path.push_back(Point2D(130,189));
			path.push_back(Point2D(125,190));
			path.push_back(Point2D(120,191));
			path.push_back(Point2D(115,192));
			path.push_back(Point2D(110,193));
			path.push_back(Point2D(105,194));
			path.push_back(Point2D(100,195));
			path.push_back(Point2D(95,196));
			path.push_back(Point2D(85,197));
			path.push_back(Point2D(75,198));
			path.push_back(Point2D(70,199));

			return path;
		}


		Path2D getGestureStar()
		{
			Path2D path;
			path.push_back(Point2D(75,250));
			path.push_back(Point2D(75,247));
			path.push_back(Point2D(77,244));
			path.push_back(Point2D(78,242));
			path.push_back(Point2D(79,239));
			path.push_back(Point2D(80,237));
			path.push_back(Point2D(82,234));
			path.push_back(Point2D(82,232));
			path.push_back(Point2D(84,229));
			path.push_back(Point2D(85,225));
			path.push_back(Point2D(87,222));
			path.push_back(Point2D(88,219));
			path.push_back(Point2D(89,216));
			path.push_back(Point2D(91,212));
			path.push_back(Point2D(92,208));
			path.push_back(Point2D(94,204));
			path.push_back(Point2D(95,201));
			path.push_back(Point2D(96,196));
			path.push_back(Point2D(97,194));
			path.push_back(Point2D(98,191));
			path.push_back(Point2D(100,185));
			path.push_back(Point2D(102,178));
			path.push_back(Point2D(104,173));
			path.push_back(Point2D(104,171));
			path.push_back(Point2D(105,164));
			path.push_back(Point2D(106,158));
			path.push_back(Point2D(107,156));
			path.push_back(Point2D(107,152));
			path.push_back(Point2D(108,145));
			path.push_back(Point2D(109,141));
			path.push_back(Point2D(110,139));
			path.push_back(Point2D(112,133));
			path.push_back(Point2D(113,131));
			path.push_back(Point2D(116,127));
			path.push_back(Point2D(117,125));
			path.push_back(Point2D(119,122));
			path.push_back(Point2D(121,121));
			path.push_back(Point2D(123,120));
			path.push_back(Point2D(125,122));
			path.push_back(Point2D(125,125));
			path.push_back(Point2D(127,130));
			path.push_back(Point2D(128,133));
			path.push_back(Point2D(131,143));
			path.push_back(Point2D(136,153));
			path.push_back(Point2D(140,163));
			path.push_back(Point2D(144,172));
			path.push_back(Point2D(145,175));
			path.push_back(Point2D(151,189));
			path.push_back(Point2D(156,201));
			path.push_back(Point2D(161,213));
			path.push_back(Point2D(166,225));
			path.push_back(Point2D(169,233));
			path.push_back(Point2D(171,236));
			path.push_back(Point2D(174,243));
			path.push_back(Point2D(177,247));
			path.push_back(Point2D(178,249));
			path.push_back(Point2D(179,251));
			path.push_back(Point2D(180,253));
			path.push_back(Point2D(180,255));
			path.push_back(Point2D(179,257));
			path.push_back(Point2D(177,257));
			path.push_back(Point2D(174,255));
			path.push_back(Point2D(169,250));
			path.push_back(Point2D(164,247));
			path.push_back(Point2D(160,245));
			path.push_back(Point2D(149,238));
			path.push_back(Point2D(138,230));
			path.push_back(Point2D(127,221));
			path.push_back(Point2D(124,220));
			path.push_back(Point2D(112,212));
			path.push_back(Point2D(110,210));
			path.push_back(Point2D(96,201));
			path.push_back(Point2D(84,195));
			path.push_back(Point2D(74,190));
			path.push_back(Point2D(64,182));
			path.push_back(Point2D(55,175));
			path.push_back(Point2D(51,172));
			path.push_back(Point2D(49,170));
			path.push_back(Point2D(51,169));
			path.push_back(Point2D(56,169));
			path.push_back(Point2D(66,169));
			path.push_back(Point2D(78,168));
			path.push_back(Point2D(92,166));
			path.push_back(Point2D(107,164));
			path.push_back(Point2D(123,161));
			path.push_back(Point2D(140,162));
			path.push_back(Point2D(156,162));
			path.push_back(Point2D(171,160));
			path.push_back(Point2D(173,160));
			path.push_back(Point2D(186,160));
			path.push_back(Point2D(195,160));
			path.push_back(Point2D(198,161));
			path.push_back(Point2D(203,163));
			path.push_back(Point2D(208,163));
			path.push_back(Point2D(206,164));
			path.push_back(Point2D(200,167));
			path.push_back(Point2D(187,172));
			path.push_back(Point2D(174,179));
			path.push_back(Point2D(172,181));
			path.push_back(Point2D(153,192));
			path.push_back(Point2D(137,201));
			path.push_back(Point2D(123,211));
			path.push_back(Point2D(112,220));
			path.push_back(Point2D(99,229));
			path.push_back(Point2D(90,237));
			path.push_back(Point2D(80,244));
			path.push_back(Point2D(73,250));
			path.push_back(Point2D(69,254));
			path.push_back(Point2D(69,252));

			return path;
		}

		Path2D getGestureTriangle()
		{
			Path2D path;
			path.push_back(Point2D(137,139));
			path.push_back(Point2D(135,141));
			path.push_back(Point2D(133,144));
			path.push_back(Point2D(132,146));
			path.push_back(Point2D(130,149));
			path.push_back(Point2D(128,151));
			path.push_back(Point2D(126,155));
			path.push_back(Point2D(123,160));
			path.push_back(Point2D(120,166));
			path.push_back(Point2D(116,171));
			path.push_back(Point2D(112,177));
			path.push_back(Point2D(107,183));
			path.push_back(Point2D(102,188));
			path.push_back(Point2D(100,191));
			path.push_back(Point2D(95,195));
			path.push_back(Point2D(90,199));
			path.push_back(Point2D(86,203));
			path.push_back(Point2D(82,206));
			path.push_back(Point2D(80,209));
			path.push_back(Point2D(75,213));
			path.push_back(Point2D(73,213));
			path.push_back(Point2D(70,216));
			path.push_back(Point2D(67,219));
			path.push_back(Point2D(64,221));
			path.push_back(Point2D(61,223));
			path.push_back(Point2D(60,225));
			path.push_back(Point2D(62,226));
			path.push_back(Point2D(65,225));
			path.push_back(Point2D(67,226));
			path.push_back(Point2D(74,226));
			path.push_back(Point2D(77,227));
			path.push_back(Point2D(85,229));
			path.push_back(Point2D(91,230));
			path.push_back(Point2D(99,231));
			path.push_back(Point2D(108,232));
			path.push_back(Point2D(116,233));
			path.push_back(Point2D(125,233));
			path.push_back(Point2D(134,234));
			path.push_back(Point2D(145,233));
			path.push_back(Point2D(153,232));
			path.push_back(Point2D(160,233));
			path.push_back(Point2D(170,234));
			path.push_back(Point2D(177,235));
			path.push_back(Point2D(179,236));
			path.push_back(Point2D(186,237));
			path.push_back(Point2D(193,238));
			path.push_back(Point2D(198,239));
			path.push_back(Point2D(200,237));
			path.push_back(Point2D(202,239));
			path.push_back(Point2D(204,238));
			path.push_back(Point2D(206,234));
			path.push_back(Point2D(205,230));
			path.push_back(Point2D(202,222));
			path.push_back(Point2D(197,216));
			path.push_back(Point2D(192,207));
			path.push_back(Point2D(186,198));
			path.push_back(Point2D(179,189));
			path.push_back(Point2D(174,183));
			path.push_back(Point2D(170,178));
			path.push_back(Point2D(164,171));
			path.push_back(Point2D(161,168));
			path.push_back(Point2D(154,160));
			path.push_back(Point2D(148,155));
			path.push_back(Point2D(143,150));
			path.push_back(Point2D(138,148));
			path.push_back(Point2D(136,148));

			return path;
		}
	};
}

#endif