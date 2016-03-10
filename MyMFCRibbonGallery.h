
/*
 * Code to allow me to have a wider image gallery in the ribbon. All of the 
 * code is here to avoid having a mostly empty header for this.
 */
 
const int nImageMargin = 4;
const int nBorderMarginY = 3;
const int MaximumGalleryImageCount = 20;

class CMyMFCRibbonGallery : public CMFCRibbonGallery
{
	public:
	CMyMFCRibbonGallery(UINT nID, LPCTSTR lpszText, int nSmallImageIndex, int nLargeImageIndex, UINT uiImagesPaletteResID, int cxPaletteImage) :
		CMFCRibbonGallery(nID, lpszText, nSmallImageIndex, nLargeImageIndex, uiImagesPaletteResID, cxPaletteImage)
	{
	}

	CSize GetRegularSize(CDC* pDC)
	{
		ASSERT_VALID(this);

		const CSize sizeImage = GetIconSize();
		CSize sizePanelSmallImage(16, 16);

		if (m_pParent != NULL)
		{
			ASSERT_VALID(m_pParent);
			sizePanelSmallImage = m_pParent->GetImageSize(FALSE);
		}

		m_bSmallIcons = (sizeImage.cx <= sizePanelSmallImage.cx * 3 / 2);

		if (m_bResetColumns && !m_bSmallIcons)
		{
			m_nPanelColumns = MaximumGalleryImageCount;

			if (m_pParentMenu != NULL && m_pParentMenu->GetCategory() == NULL)
			{
				// From the default panel button
				m_nPanelColumns = 3;
			}
		}

		m_bResetColumns = FALSE;

		if (IsButtonLook())
		{
			return CMFCRibbonButton::GetRegularSize(pDC);
		}

		if (m_arIcons.GetSize() == 0)
		{
			CreateIcons();
		}

		ASSERT_VALID(m_pParent);

		const CSize sizePanelLargeImage = m_pParent == NULL ? 
			CSize (0, 0) : m_pParent->GetImageSize(TRUE);

		CSize size(0, 0);

		if (m_bSmallIcons)
		{
			size.cx = sizeImage.cx * m_nPanelColumns;

			int nRows = 3;

			if (sizePanelLargeImage != CSize(0, 0) && sizeImage.cy != 0)
			{
				nRows = max(nRows, sizePanelLargeImage.cy * 2 / sizeImage.cy);
			}

			size.cy = nRows * sizeImage.cy + 2 * nBorderMarginY;
		}
		else
		{
			size.cx = (sizeImage.cx + 2 * nImageMargin) * m_nPanelColumns;
			size.cy = sizeImage.cy + 3 * nImageMargin + 2 * nBorderMarginY;
		}

		//---------------------------------------
		// Add space for menu and scroll buttons:
		//---------------------------------------
		size.cx += GetDropDownImageWidth() + 3 * nImageMargin;

		return size;
	}
};