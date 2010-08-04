package com.anand.embvid.embvid;


//InitStaticArray
class FreqSet
{
	Integer MinFreq;
	Integer MaxFreq;
	Integer Bandwidth;
	public FreqSet (Integer min, Integer max, Integer bw)
	{
		this.MinFreq = min;
		this.MaxFreq = max;
		this.Bandwidth = bw;
	}
}


class FreqList 
{
	String countryName;
	FreqSet freqList[];
	public FreqList(String s, FreqSet fl[])
	{
		this.countryName = s;
		this.freqList = fl;
	}
}

class TotalFreqList
{
 
	FreqSet VHF_6[] = new FreqSet[]{
	   new FreqSet(57000,69000,6000),
	   new FreqSet(79000,85000,6000),
	   new FreqSet(177000,207000,6000)
   };
	FreqSet VHF_7[] = new FreqSet[]{
		new FreqSet(177500,226500,7000)
	};
	FreqSet VHF_8[] = new FreqSet[]{
			new FreqSet(178750,218750,8000)
		};

   FreqList freqList[];
   
   public TotalFreqList()
   {
	   freqList = new FreqList[]{
				new FreqList("VHF_6", VHF_6),
				new FreqList("VHF_7", VHF_7),
				new FreqList("VHF_8", VHF_8),
				
		};
   }
}

/*
static struct FREQ_LIST_T_TAG UHF_6[] = {
    {473000,887000,6000}
};
static struct FREQ_LIST_T_TAG UHF_7[] = {
    {529500,816500,7000}
};
static struct FREQ_LIST_T_TAG UHF_8[] = {
    {474000,858000,8000}
};

static struct FREQ_LIST_T_TAG FRANCE[] = {
    {474000,858000,8000},
    {529500,816500,7000},
    {473000,887000,6000}
};

static struct FREQ_LIST_T_TAG ITALY[] = {
    {177500,194500,7000},
    {203500,212500,7000},
    {219500,226500,7000},
    {474000,858000,8000}
};

static struct FREQ_LIST_T_TAG TAIWAN[] = {
    {527000,593000,6000}
};

static struct FREQ_LIST_T_TAG SWEDEN[] = {
    {177500,226500,7000},
    {474000,514000,8000},
    {519000,519000,8000},
    {522000,618000,8000},
    {623500,623500,8000},
    {626000,858000,8000}
};

static struct FREQ_LIST_T_TAG US_ATSC[] = {
    { 57000, 69000,6000},
    { 79000, 85000,6000},
    {177000,213000,6000},
    {473000,803000,6000}
};

static struct FREQ_LIST_T_TAG TAWAIN_ATSC[] = {
    {473000,803000,6000}
};
*/
	
/*
class FreqList {

	static Integer[] integerArray;

	static {
	  integerArray= new Integer[] {
	    new Integer(1),
	    new Integer(2),
	    new Integer(3),
	    new Integer(4),
	    };
	}

	  public static void main(String args[]) {
	    for (int i=0; i < integerArray.length; i++){
	    System.out.println(integerArray[i]);
	    }
	  }
	}
	*/
