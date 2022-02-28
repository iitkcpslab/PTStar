#include<bits/stdc++.h>
#include"planner_variables.h"
using namespace std;
using namespace planner_info;
int prefs,tune;
vector<int> prod_states;
vector<int> no_of_tns,no_of_states;
vector<vector<int>>  prefs_dest(10);

/***djkistra with qtot system states with loops,until,next,few corrections,integer hash improvements,consider node in productgraph for book-keeping***/
//correct the adj updations

bool has_duplicate(vector<int> v)
{
  set<int> s;
  for (auto i = v.begin(); i != v.end(); ++i) {
    int ai = abs(*i);
    if (s.count(ai)) return true;
    s.insert(ai);
  }
  return false;
}


unordered_map<unsigned long long int,vector<prod_graph_node> > node_neighbour;
clock_t t;

float cal_actual_distance_bw_points(point a,point b);

pair<int,int> compute_suffix_cycles();

int state_key(int p,int s)
{
   return p*10+s;
}
int l=10000;
int power(int x, int y) 
{  
   int i, power = 1;
   if(y==0)
      return 1;
   for(i=1;i<=y;i++)
      power = power*x;
   return power;
} 

pair<int,int> createtrans(char tpc[],int count)
{

  string s="",filename;
  const int BUFSIZE=128;
  char buf[BUFSIZE];
  char buff[1000];
  strcpy(buff,tpc);
  FILE *fp;
  ofstream outfile;
  filename="trans"+to_string(count)+".dat";
  outfile.open(filename);
  int i,j,k,cnt=0;
  pair<int,int> rval;

  if ((fp = popen(buff, "r")) == NULL) 
  {
    printf("Error opening pipe!\n");
    return rval;
  }
    /*if ((fp = fopen("result3.txt", "r")) == NULL)
    {
        printf("Error opening pipe!\n");
        return rval;
    }*/

  while (fgets(buf, BUFSIZE, fp) != NULL) 
  {
        // Do whatever you want here...
        //printf("%s", buf);
    s = s+buf;
  }

  if(pclose(fp))  
  {
    printf("Command not found or exited with error status\n");
    return rval;
  }
  //cout<<"\n";
  map<string,int> mp;
  map<string,int> state;
  vector<string> st;

    //cout<<s;
  string tmp;
  i=0;
    //cout<<s.size();
  while(s[i]!='T' && s[i]!='a')
    i++;
  int stno=0;
  vector<string> table;
  while(i<s.size())
  {
    tmp="";
    while(i<s.size() && s[i]!=':')
    {
      tmp+=s[i];
      i++;
    }
    if(tmp[0]=='}')
      break;
    if(state.find(tmp)==state.end())
    {
      state[tmp] = stno;
      st.push_back(tmp);
      stno++;
    }

     while(i<s.size() && tmp!="fi;")
    {
    tmp = "";
    while(i<s.size() && s[i]!='\t' && s[i]!=' ' && s[i]!='\n')
    {
     if(s[i]!='(' && s[i]!=')')
       tmp+=s[i];
     i++;
    }
    if(tmp!=" " && tmp!="" && tmp!="if" && tmp!="::" && tmp!="->" && tmp!="goto" &&  tmp!="fi;")
    table.push_back(tmp);
    i++;
    }

  }

  dest=vector<int>(0);
  for(i=0;i<st.size();i++)
  {
    mp[st[i]]=i;
    if(st[i][0]=='a' && st[i][1]=='c' && st[i][2]=='c')
    {
      dest.push_back(i);
      prefs_dest[count].push_back(i);
    }
  }

  vector<vector<int> > vec;
  vector<int> tmpvec;
  int x,y;
  k=0;
  i=1;

  while(i<table.size())
  {
  if(table[i][0]==':')
  {
    k++;
  }
  else if(table[i]=="||")
  {
   vec.push_back(tmpvec);
   tmpvec = vector<int>(0);
  }
  else if(table[i]=="&&")
  {
  }
  else if(mp.find(table[i])!=mp.end())
  {
    vec.push_back(tmpvec);
    for(x=0;x<vec.size();x++)
    {
      int cost=power(tune,prefs-count);
      
      std::vector<int>::iterator it;
      it = std::find (dest.begin(), dest.end(), mp[table[i]]);
      if(it != dest.end())
	cost=0;
      if(count==prefs)
	cost=0;
      //cout<<k<<" "<<cost<<" "<<count<<"\n";
      cnt++;
      outfile<<(k+1)<<" "<<(mp[table[i]]+1)<<" "<<cost<<" "<<vec[x].size()<<" ";
      for(y=0;y<vec[x].size();y++)
      {
        outfile<<vec[x][y]<<" ";
      }
      outfile<<"\n";
    }
    vec = vector<vector<int> >(0);
    tmpvec = vector<int>(0);
  }
  else if(table[i]=="1")
  {
    tmpvec.push_back(0);
  }
  else if(table[i][0]=='!')
  {
  int v=0;
  for(x=2;x<table[i].size();x++)
    v = v*10+(table[i][x]-'0');
  tmpvec.push_back(-1*v);
  }
  else if(table[i][0]=='p')
  {
   int v=0;
   for(x=1;x<table[i].size();x++)
     v = v*10+(table[i][x]-'0');
   tmpvec.push_back(v);
  }
  else if(table[i]=="true")
  {
     tmpvec.push_back(0);
  }
  else if(table[i]=="skip")
  {
    int cost=power(tune,prefs-count);
    std::vector<int>::iterator it;
    it = std::find (dest.begin(), dest.end(), mp[table[i]]);
    if(it != dest.end())
      cost=0;
    cnt++; 
    outfile<<k+1<<" "<<k+1<<" "<<cost<<" 1 0\n";
  }

  i++;
}
outfile.close();

rval.first = st.size();
rval.second = cnt;
return rval;
}

int create_cost_graph(int count)
{
    fstream ifile,prodfile;
    ofstream costfile;
    int s1,s2,p1,p2,temp,i,j,cost,prod_cost;
    string filename;
   int reg=1,start,end,file,k;
   //cout<<"hoiii\n";
   //prefs_dest = vector<vector<int>>(prefs+1); 
   //trans = vector< vector< vector< vector<int> > > >(qtot,vector< vector< vector<int> > >(qtot) );
   for(file=1;file<count-1;file++)
   {	
	filename="cost"+to_string(file-1)+".dat";
	if(file==1)
	  filename="trans0.dat";	
	prodfile.open(filename);
	filename="cost"+to_string(file)+".dat";
	costfile.open(filename);
	reg=reg*no_of_tns[file-1];
	for(i=0;i<reg;i++)
	{
		//calcout<<"hoiii\n";
	   prodfile>>p1;
	   prodfile>>p2;
	   prodfile>>prod_cost;
	   //cout<<p1<<" "<<p2<<" ";
	   int len=0;
           prodfile>>len;
	   vector<int> temp_vec;
	   for(k=0;k<len;k++)
	   {
	       prodfile>>temp;
	       temp_vec.push_back(temp);
	   }
	   filename="trans"+to_string(file)+".dat";   
	   ifile.open(filename);
	   for(j=0;j<no_of_tns[file];j++)
	   {
		ifile>>s1;
		ifile>>s2;
		ifile>>cost;
		//cout<<s1<<" "<<s2<<"\n";
		start=state_key(p1,s1);
		end=state_key(p2,s2);
		vector<int> fin_vec;
                for(k=0;k<len;k++)
	           fin_vec.push_back(temp_vec[k]);
		int len1=0;
		ifile>>len1;
		for(k=0;k<len1;k++)
	   	{
		   ifile>>temp;
		   fin_vec.push_back(temp);
		}
                std::sort(fin_vec.begin(), fin_vec.end());
		fin_vec.erase(std::unique(fin_vec.begin(), fin_vec.end()), fin_vec.end());
		int s=0;
		std::vector<int>::iterator it;
		it = std::find (fin_vec.begin(), fin_vec.end(), 0);
		//if(has_duplicate(fin_vec))
		//	continue;
		if(fin_vec.size()!=1 && it != fin_vec.end())
		{s=-1;}
		costfile<<start<<" "<<end<<" "<<prod_cost+cost<<" "<<fin_vec.size()+s;
		for(k=0;k<fin_vec.size();k++)
		  if(k != it - fin_vec.begin() || fin_vec.size()==1)
		    costfile<<" "<<fin_vec[k];
		costfile<<"\n";
		/*if(file == count-2 || count==1)
		{
			cout<<start<<" "<<end<<" ";
			int cnt=0;
			for(int xy=0;xy<fin_vec.size();xy++)
			{
				if(fin_vec[xy]<0)
					cnt++;
				cout<<fin_vec[xy]<<" ";
			}
			if(cnt==fin_vec.size())
				neg_trans_filter[start][end]=1;
			cout<<"\n count value:- "<<cnt<<"\n";
					
		}*/		
	   }
	   ifile.close();
	}
	prodfile.close();
	costfile.close();
   }
   return reg*no_of_tns[file-1];
}


void initializegrid(char *grid_info_file)
{
    string s,buf;
    int i,j,k,no_of_trans,literal,num_obstacles,prod_reg;
    pair<int,int> rval;
    /////////////////////////////////////
 
    fstream ft;
    ft.open("query.dat", ios::in);
    ft>>tune;
    ft>>prefs;
    cout<<"no of preferences "<<prefs<<"\n";
    int count=0;
    getline(ft,buf);
    while(getline(ft,buf))
    {
	
	char buff[1000];
	strcpy(buff,buf.c_str());
	rval = createtrans(buff,count);
	no_of_states.push_back(rval.first);
	no_of_tns.push_back(rval.second);
        count++;
    }
    ft.close();

    prod_reg=create_cost_graph(count);
    
  //cout<<prod_reg<<"--------\n";

    //t = clock();

    no_of_trans = rval.second;
    qtot = rval.first;

    /**vector for storing the automata transition table**/
    trans = vector< vector< vector< vector<int> > > >(qtot,vector< vector< vector<int> > >(qtot) );
    /**storing automata transitions on conjunction of negative literals**/
    negtrans = vector< vector< vector< vector<int> > > >(qtot,vector< vector< vector<int> > >(qtot) );
    neg_trans_to_neighbour = vector<int>(qtot);
    ifstream ifile;
    string str="trans"+to_string(prefs)+".dat";
    ifile.open(str);
    cout<<"Number of automata_states="<<qtot<<"\n";
    cout<<"Number of automata_transitions="<<no_of_trans<<"\n";
    /**reading automata transitions**/
    vector<int> automata_states;
    vector<int> transition_condn;
    int cst,val;
    for(i=0;i<no_of_trans;i++)
    {
        transition_condn = vector<int>(0);
        automata_states = vector<int>(2);
        ifile>>val;
        automata_states[0]=val-1;
        ifile>>val;
        automata_states[1]=val-1;
        ifile>>cst;
        int trans_condn_len=0,neg_literals=0,strict_neg_literals=0;
        ifile>>trans_condn_len;
        for(j=0;j<trans_condn_len;j++)
        {
            ifile>>literal;
            transition_condn.push_back(literal);
            if(literal <= 0)
                neg_literals++;
            if(literal < 0)
                strict_neg_literals++;
        }
        /**storing transition condition  in automata**/
        trans[automata_states[0]][automata_states[1]].push_back(transition_condn);

        /**if transition condition is conjunction of negative literals**/ 
        if(neg_literals==trans_condn_len)
        {
            if(automata_states[0]!=automata_states[1] && strict_neg_literals==trans_condn_len)
            neg_trans_to_neighbour[automata_states[0]]=1;
            negtrans[automata_states[0]][automata_states[1]].push_back(transition_condn);
        }

    }
    ifile.close();
    /////////////////////////////////////

    
    ////////////////////////////////////////////////////////////

    ifstream grid_file;
    grid_file.open(grid_info_file);
    int x,y;
    grid_file>>nrow;
    grid_file>>ncol;
    sz = max(nrow,ncol);

    grid_file>>num_obstacles;
    grid_sz = sz;
    sz = max(sz,qtot);
    /*storing obstacle coordinates*/
    for(i=0;i<num_obstacles;i++)
    {
        grid_file>>y;
        grid_file>>x;
        grid[y*sz+x]=-1;
    }
    
    int num_pos_system_states;
    vector<int> grid_state;
    grid_file>>num_pos_system_states;
    
    /** reading coordinates of states and proposition true at it **/
    for(i=0;i<num_pos_system_states;i++)
    {
        grid_state = vector<int>(2);
        grid_file>>grid_state[0];
        grid_file>>grid_state[1];
        grid_file>>literal;
        s = conv_vec_to_string(grid_state);
        /**pushing the proposition true at state s**/
        prop[s].push_back(literal);
        /**storing cells associated with a proposition**/
        prop_sys_states[literal].push_back(grid_state);
        /**storing the list of states with a proposition true at it**/
        if(prop[s].size()==1)
            pos_system_state.push_back(grid_state);
    }
    grid_file.close();
///////////////////////////////////
    if(prefs==1)
	str="trans0.dat";
    else
        str="cost"+to_string(prefs-1)+".dat";
    //str="cost"+to_string(prefs-1)+".dat";
    //cout<<str<<"......\n";
    //ifstream ifile;
    ifile.open(str);
    int inc=0;
    //cout<<inc;
    vector<int> v;
    vector<int> p;
    for(i=0;i<prod_reg;i++)
    {
        v = vector<int>(0);
        p = vector<int>(0);
        ifile>>val;
	//cout<<val<<"\n";
        p.push_back(val);
	prod_states.push_back(val);
        ifile>>val;
        //cout<<val<<"\n";
        p.push_back(val);
	prod_states.push_back(val);
	ifile>>cst;
	//cout<<cst;
	trans_cost[p[1]]=cst;
        int len=0;
        ifile>>len;
	int neg_literals=0,strict_neg_literals=0;
        for(j=0;j<len;j++)
        {
            ifile>>val;
            v.push_back(val);
	    if(val <= 0)
                neg_literals++;
            if(val < 0)
                strict_neg_literals++;
        }
        cost_filter[p[0]][p[1]].push_back(v); 
	if(neg_literals==len)
        {
            //if(automata_states[0]!=automata_states[1] && strict_neg_literals==len)
            //neg_trans_to_neighbour[automata_states[0]]=1;
            neg_trans_filter[p[0]][p[1]].push_back(v);
        }
	
	/*int cnt=0;
	//cout<<"\n";
	for(int xy=0;xy<v.size();xy++)
	{
		if(v[xy]<0)
		  cnt++;
		//cout<<v[xy]<<" ";
		
	}
	if(v.size()==cnt && cnt!=0)
	{
	  neg_trans_filter[p[0]][p[1]]=1;
	  //cout<<"\n neg trans on "<<p[0]<<" "<<p[1]<<"\n";
	}
	else
	  neg_trans_filter[p[0]][p[1]]=0;*/	
	
    }
    ifile.close();

    vector<int>::iterator ip;
    sort(prod_states.begin(), prod_states.end());
    ip = unique(prod_states.begin(), prod_states.end());
    prod_states.resize(distance(prod_states.begin(), ip));
    
    for(int pro=0;pro<prod_states.size();pro++)
	cout<<" "<<prod_states[pro];
    cout<<"\n";

    adj = vector<unordered_map<int,float> >(100);
    updated = vector<unordered_map<int,int> >(100);
}

float cal_actual_distance_bw_points(point srcnode,point dest_node)
{
    priority_queue<trans_system_node,vector<trans_system_node>,trans_system_node_cost_comparator> qopen;
    unordered_map<long long int,float> node_dist_from_src;
    unordered_map<long long int,int> init;
    unordered_map<long long int,int> visited;

    trans_system_node start_node;
    start_node.coord  = srcnode;
    start_node.g = 0;
    start_node.h = cal_heuristic_cost(start_node.coord,dest_node);
    start_node.f = start_node.g+start_node.h;
    qopen.push(start_node);
    long long int coord_value = srcnode.y*sz+srcnode.x;
    init[coord_value]=1;
    node_dist_from_src[coord_value]=0;
    //if(!valid(dest_node.y,dest_node.x))
	//return -1;
    while(!qopen.empty())
    {
        trans_system_node nd = qopen.top();
        qopen.pop();
        point pt = nd.coord;
        coord_value = pt.y*sz+pt.x;

        if(nd.coord.x==dest_node.x && nd.coord.y==dest_node.y)
        {
		//cout<<"\n"<<nd.f<<"\n";
            return nd.f;
        }

        if(visited[coord_value])
            continue;
        else
            visited[coord_value]=1;

        //cout<<"\n"<<pt.y<<","<<pt.x<<","<<nd.f<<" : ";

        point nbh;

        for(int i=0;i<dir;i++)
        {

            if(!valid(pt.y+nb[i][0],pt.x+nb[i][1]) && ((pt.y+nb[i][0])!=dest_node.y || (pt.x+nb[i][1]!=dest_node.x)))
                continue;
            if( abs(nb[i][0])+abs(nb[i][1])==2 )// (!valid(pt.y+nb[i][0],pt.x) || !valid(pt.y,pt.x+nb[i][1]) ) )
                continue;
            //transition
            nbh.y =  pt.y+nb[i][0];
            nbh.x =  pt.x+nb[i][1];
            trans_system_node tmp;
            coord_value = nbh.y*sz+nbh.x;
            tmp.g = nd.g + ( (abs(nbh.y-pt.y)+abs(nbh.x-pt.x))>1?1.5:1);
            if(!init[coord_value])
            {
                tmp.coord = nbh;
                tmp.h = cal_heuristic_cost(nbh,dest_node);
                tmp.f = tmp.g+tmp.h;
                //cout<<" "<<nbh.y<<","<<nbh.x<<" ";
                qopen.push(tmp);
                init[coord_value]=1;
                node_dist_from_src[coord_value]=tmp.g;
            }
            else
            {
                if(tmp.g < node_dist_from_src[coord_value])
                {
                    tmp.coord = nbh;
                    tmp.h = cal_heuristic_cost(nbh,dest_node);
                    tmp.f = tmp.g+tmp.h;
                    //cout<<" "<<nbh.y<<","<<nbh.x<<" ";
                    qopen.push(tmp);
                    node_dist_from_src[coord_value]=tmp.g;
                }

            }
        }

    }

    return -1;
}


/*float cal_actual_distance_bw_points(point srcnode,point dest_node)
{
    priority_queue<trans_system_node,vector<trans_system_node>,trans_system_node_cost_comparator> qopen;
    unordered_map<long long int,float> node_dist_from_src;
    unordered_map<long long int,int> init;
    unordered_map<long long int,int> visited;

    int n_prod_st = par;
    trans_system_node start_node;
    start_node.coord  = srcnode;
    start_node.g = 0;
    start_node.h = cal_heuristic_cost(start_node.coord,dest_node);
    start_node.f = start_node.g+start_node.h;
    start_node.cfstate = par;
    //start_node.t_cst=t_cst;
    qopen.push(start_node);
    long long int coord_value = par*sz*sz+srcnode.y*sz+srcnode.x;
    init[coord_value]=1;
    //cout<<"in caluculating distance \n";
    node_dist_from_src[coord_value]=0;
    /*if(start_node.coord.x==dest_node.x && start_node.coord.y==dest_node.y && start_node.cfstate==cf)
    {
	int k= trans_cost[cf];
	//cout<<k<<"\n";
	return k;
    }
    
    while(!qopen.empty())
    {
      trans_system_node nd = qopen.top();
      qopen.pop();
      point pt = nd.coord;
      n_prod_st = nd.cfstate;
      coord_value = nd.cfstate*sz*sz+pt.y*sz+pt.x;

	//cout<<nd.coord.y<<","<<nd.coord.x<<" "<<nd.cfstate<<" "<<nd.f<<"\n";
      if( nd.coord.x==dest_node.x && nd.coord.y==dest_node.y && nd.cfstate==cf)
      {
	  //cout<<"("<<srcnode.y<<","<<srcnode.x<<")  ("<<dest_node.y<<","<<dest_node.x<<")  "<<nd.f<<"\n";
          return nd.f;
      }

      if(visited[coord_value])
          continue;
      else
         visited[coord_value]=1;

        //cout<<"\n"<<pt.y<<","<<pt.x<<","<<nd.f<<" : ";

      point nbh;
      //cout<<" neighbour states start \n";
      for(int i=0;i<dir;i++)
      {
	for(int pro=0;pro<prod_states.size();pro++)
        {
          if(cost_filter[n_prod_st].find(prod_states[pro])==cost_filter[n_prod_st].end())
	  	continue;
          for(int lp=0;lp<cost_filter[n_prod_st][prod_states[pro]].size();lp++)
          {
	    vector<int> transition_req = cost_filter[n_prod_st][prod_states[pro]][lp];
            if(!valid(pt.y+nb[i][0],pt.x+nb[i][1]) && ((pt.y+nb[i][0])!=dest_node.y || (pt.x+nb[i][1]!=dest_node.x)))
                continue;
            if( abs(nb[i][0])+abs(nb[i][1])>=2 )// || ( !valid(pt.y+nb[i][0],pt.x) || !valid(pt.y,pt.x+nb[i][1]) ) )
                continue;
            //transition
            nbh.y =  pt.y+nb[i][0];
            nbh.x =  pt.x+nb[i][1];
	    vector<int> neigh_grid_cell{nbh.y,nbh.x};
	    string nbh_s = conv_vec_to_string(neigh_grid_cell);
	    vector<int> nxtstate;
	    vector<int> prop_nbh;
	   //finding the propositions true at neighbour cell nbh in grid//
	                if(prop.find(nbh_s)==prop.end())
	                {
	                    prop_nbh.push_back(0);
	                }
	                else
	                {
	                    prop_nbh = prop[nbh_s];
	                    prop_nbh.push_back(0);
	                }

	                int trans_literals_satisfied=0;
	                for(int k=0;k<transition_req.size();k++)
	                {
	                    if(transition_req[k]>=0)
	                        for(int l=0;l<prop_nbh.size();l++)
	                        {
	                            if(transition_req[k]==prop_nbh[l])
	                            {
	                            trans_literals_satisfied++; break;
	                            }
	                        }
	                    else
	                    {
	                        int satisfied_neg_literal=1;
	                        for(int l=0;l<prop_nbh.size();l++)
	                        {
	                            if(-1*transition_req[k]==prop_nbh[l])
	                            {
	                                satisfied_neg_literal=0; break;
	                            }
	                        }
	                        trans_literals_satisfied+=satisfied_neg_literal;
	                    }
	                }

	    if(trans_literals_satisfied==transition_req.size())
	    {
	     
            trans_system_node tmp;
	    
            coord_value = prod_states[pro]*sz*sz+nbh.y*sz+nbh.x;
	    //float l = trans_cost[prod_states[pro]];
             
	    float k= ((abs(nbh.y-pt.y)+abs(nbh.x-pt.x))>1?1.5:1);

	    tmp.g = nd.g + k ;//+ l - nd.t_cst;
	    //cout<<nbh.y<<" "<<nbh.x<<" "<<prod_states[pro]<<"\n";
            if(!init[coord_value])
            {
                tmp.coord = nbh;
		tmp.cfstate = prod_states[pro];
		//tmp.t_cst = l;
                tmp.h = cal_heuristic_cost(nbh,dest_node);
		tmp.f = tmp.g+tmp.h;
                //cout<<" "<<nbh.y<<","<<nbh.x<<" ";
                qopen.push(tmp);
                init[coord_value]=1;
                node_dist_from_src[coord_value]=tmp.g;
            }
            else
            {
                if(tmp.g < node_dist_from_src[coord_value])
                {
                    tmp.coord = nbh;
		    tmp.cfstate = prod_states[pro];
		    //tmp.t_cst = l;//power(prefs,prefs+2);
                    tmp.h = cal_heuristic_cost(nbh,dest_node);
		    tmp.f = tmp.g+tmp.h;
                    //cout<<" "<<nbh.y<<","<<nbh.x<<" ";
                    qopen.push(tmp);
                    node_dist_from_src[coord_value]=tmp.g;
                }

            }

	    }
        }
      }
     }
    //cout<<" neighbour states ends \n";
    
    }

    return -1;
}*/

int print_astar_computed_path(trans_system_path_node* nd)
{
    if(nd!=NULL)
    {
        //pathlen++;
        int path_len = print_astar_computed_path(nd->par);
        //cout<<"**"<<nd->coord.y<<","<<nd->coord.x<<","<<nd->state<<","<<nd->cfstate<<","<<nd->t_cst<<"\n";
        //point1 temp{nd->coord.y,nd->coord.x,nd->state,nd->cfstate,nd->f};
        cur_path.push_back(nd->coord);
        return path_len+1;
    }
    return 0;
}

/**adding obstacles to the grid**/
void copy_map(unordered_map<long long int,int> &obs,int v)
{
    unordered_map<long long int,int>::iterator it;
    for(it = obs.begin();it!=obs.end();it++)
    {
        if(it->second)
        grid[it->first]=v;
    }
}

int astar_path_src_to_dest(prod_graph_node* src,prod_graph_node* dest)
{
    /////////calculate path from src to dest/////////////////////
    unordered_map<long long int,trans_system_path_node* > init_vertex;
    unordered_map<long long int,int > vis;
    int chng=0;

    trans_system_path_node* dst = newtrans_system_path_node(src->coord.x,src->coord.y,src->state,src->cfstate);

    priority_queue<trans_system_path_node*,vector<trans_system_path_node*>,trans_system_path_node_cost_comparator> qopen;
    vector<trans_system_path_node*> closed;
    qopen.push(dst);

    vector<int> src_vertex{src->coord.y,src->coord.x,src->state,src->cfstate};
    
    init_vertex[key(src_vertex)]=dst;
    int jt,k,it,automaton_state = src->state,undesired_prop;   
    int v;
    unordered_map<long long int,int> obs1;
    
    //marking obstacles due to self-trans-on conjunction_of_negative literals on current_automaton_state
    if(negtrans[automaton_state][automaton_state].size() > 0)
    {
        vector<int> transition_req = negtrans[automaton_state][automaton_state][0];
        for(it=0;it<transition_req.size();it++)
        {
            undesired_prop = transition_req[it]*-1;
            for(v=0;v<prop_sys_states[undesired_prop].size();v++)
            {
                obs1[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]]=-1;
            }
        }
        for(int ind=1;ind<negtrans[automaton_state][automaton_state].size();ind++)
        {
            unordered_map<long long int,int> obs2;
            vector<int> transition_req = negtrans[automaton_state][automaton_state][ind];
            for(it=0;it<transition_req.size();it++)
            {
                undesired_prop = transition_req[it]*-1;
                /**marking all states where undesired_prop is true as obstacles**/
                for(v=0;v<prop_sys_states[undesired_prop].size();v++)
                {
                    if(obs1[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]])
                        obs2[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]]=-1;
                }
            }
            obs1 = obs2;
        }
        /**unmarking all states satisfying a conjunction containing positive literal marked as obstacles**/
        for(int z=0;z<qsytrans[automaton_state][automaton_state].size();z++)
        {
            k = qsytrans[automaton_state][automaton_state][z];
            obs1[(pos_system_state[k][0])*sz+pos_system_state[k][1]]=0;
        }
	if(src->state != dest->state)
	{
		vector<int> transition_req2 = cost_filter[src->cfstate][dest->cfstate][0];
		//transition_req.insert(transition_req.end(),transition_req2.begin(),transition_req2.end());
		for(it=0;it<transition_req2.size();it++)
                {
		    undesired_prop = transition_req2[it]*-1;
		    for(v=0;v<prop_sys_states[undesired_prop].size();v++)
		    {
		         obs1[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]]=-1;
		    }
		}
		for(int z=0;z<fsytrans[src->cfstate][dest->cfstate].size();z++)
		{
		      int grid_state = fsytrans[src->cfstate][dest->cfstate][z];
		      obs1[(pos_system_state[grid_state][0])*sz+pos_system_state[grid_state][1]]=0;
		}
	}

        copy_map(obs1,-1);    
    }


    while(!qopen.empty())
    {
        trans_system_path_node* cur_nd = qopen.top();
        qopen.pop();
        point cur_pt = cur_nd->coord;
	int n_prod_st = cur_nd->cfstate,o;
        int current_automaton_state = cur_nd->state;
        vector<int> cur_node_info{cur_pt.y,cur_pt.x,current_automaton_state,cur_nd->cfstate};
        if(cur_nd->coord.x==dest->coord.x && cur_nd->coord.y==dest->coord.y)
        {
            //repfin = new_prod_graph_node(nd->coord.x,nd->coord.y,nd->state);
            cur_nd->state = dest->state;
            ////////////////////aprog-10
            print_astar_computed_path(cur_nd);
            break;
        }

        long long int cur_node_k = key(cur_node_info);
        
        if(vis.find(cur_node_k)!=vis.end())
            continue;
        else
            vis[cur_node_k]=1;
	//for(int mnp=0;mnp<l;mnp++)
	//	o=vis[cur_node_k];	
        vector<int> neigh_state;
        /** relaxing node cur_node and it's neighbours**/
        point nbh;
          for(int i=0;i<dir;i++)
          {
            if(!valid(cur_pt.y+nb[i][0],cur_pt.x+nb[i][1]) && ((cur_pt.y+nb[i][0])!=dest->coord.y || (cur_pt.x+nb[i][1]!=dest->coord.x)))
                continue;
            if( abs(nb[i][0])+abs(nb[i][1])>=2 )// || (!valid(cur_pt.y+nb[i][0],cur_pt.x) || !valid(cur_pt.y,cur_pt.x+nb[i][1]) ) )
                continue;
            //transition
            nbh.y =  cur_pt.y+nb[i][0];
            nbh.x =  cur_pt.x+nb[i][1];
              neigh_state = vector<int>(0);
              neigh_state.push_back(nbh.y);
              neigh_state.push_back(nbh.x);
              neigh_state.push_back(current_automaton_state);
	      neigh_state.push_back(src->cfstate);
	      //float l = trans_cost[prod_states[pro]];
	      float k = ((abs(nbh.y-cur_pt.y)+abs(nbh.x-cur_pt.x))>1?1.5:1);
	    trans_system_path_node* oldtmp;
              if(init_vertex.find(key(neigh_state))==init_vertex.end())
              {   //cntstate++;
		
                trans_system_path_node* neigh_node = newtrans_system_path_node(nbh.x,nbh.y,current_automaton_state,src->cfstate);                
		neigh_node->g = cur_nd->g+k;
                neigh_node->h = cal_heuristic_cost(nbh,dest->coord);
		neigh_node->f = neigh_node->g+neigh_node->h;
                neigh_node->par = cur_nd;
                //cout<<" "<<nbh.y<<","<<nbh.x<<","<<j<<","<<tmp->f<<","<<tmp->h<<","<<tmp->g<<" ";
                init_vertex[key(neigh_state)]=neigh_node;
                qopen.push(neigh_node);
              }
              else
              {
                oldtmp = init_vertex[key(neigh_state)];
                if(oldtmp->g > (cur_nd->g+k) )
                {

                    trans_system_path_node* neigh_node = newtrans_system_path_node(nbh.x,nbh.y,current_automaton_state,src->cfstate);
		    neigh_node->g = cur_nd->g+k;
                    neigh_node->h = oldtmp->h;
                    neigh_node->f = neigh_node->g+neigh_node->h;
                    neigh_node->par = cur_nd;
                    //cout<<" here "<<nd->g<<" "<<tval<<" "<<oldtmp->g<<" "<<nbh.y<<","<<nbh.x<<","<<j<<","<<tmp->f<<","<<tmp->h<<","<<tmp->g<<" ";
                    init_vertex[key(neigh_state)]=neigh_node;
                    qopen.push(neigh_node);
                }

              
	    }
            ////////////////
          
	}
    closed.push_back(cur_nd);
    }

    for(it=0;it<closed.size();it++)
    {
        delete(closed[it]);
    }
 
    copy_map(obs1,0);
}


void calsystates()
{

    int i,j,k,q,z,cnt=0,tmp_cnt=0;
    vector<int> v;
    string s;
    //TS states satifying an incoming transition to an automaton state
    qsystate = vector< vector<int> >(qtot);
    
    //TS states satifying an automaton transition
    qsytrans = vector< vector< vector<int> > >(qtot,vector< vector<int> >(qtot) );

    for(int nstate=0;nstate<qtot;nstate++)
    {
        for(j=0;j<qtot;j++)
        {
            for(z=0;z<trans[nstate][j].size();z++)
            {
                vector<int> transition_req = trans[nstate][j][z];
                if(transition_req.size()==0)
                    continue;

                for(i=0;i<pos_system_state.size();i++)
                {

                    point pt;
                    //transition
                    pt.y =  pos_system_state[i][0];
                    pt.x =  pos_system_state[i][1];

                    v = vector<int>(0);
                    v.push_back(pt.y);
                    v.push_back(pt.x);

                    vector<int> prop_cur;
                    s = conv_vec_to_string(v);
                    if(prop.find(s)==prop.end())
                    {
                        prop_cur.push_back(0);
                    }
                    else
                    {
                        prop_cur = prop[s];
                    }


                    int pos=0;
                    cnt=0;
                    for(int k=0;k<transition_req.size();k++)
                    {
                        if(transition_req[k]>=0)
                            for(int l=0;l<prop_cur.size();l++)
                            {
                                if(transition_req[k]==prop_cur[l])
                                    {cnt++; pos=1; break;}
                            }
                            else
                            {
                                int tmp=1;
                                for(int l=0;l<prop_cur.size();l++)
                                {
                                    if(-1*transition_req[k]==prop_cur[l])
                                        {tmp=0; break;}
                                }
                                cnt+=tmp;
                                if(tmp==0)
                                    break;
                            }
                    }
                        //if(pos && cnt==transition_req.size() ) added
                        //system states from self loop in automata included
                    if(pos && cnt==transition_req.size())
                    {
                        /*cout<<nstate<<" "<<j<<" "<<i<<"\n";*/
                        qsystate[j].push_back(i);
                        //qinstate[j].push_back(nstate);
                        qsytrans[nstate][j].push_back(i);
                        //tmp_cnt++;
                    }

                }
            }
        }
    }
}


void calfilstates()
{

    int i,j,k,q,z,cnt=0,tmp_cnt=0;
    vector<int> v;
    string s;
    //TS states satifying an incoming transition to an automaton state
    //fsystate = vector< vector<int> >(qtot);
    
    //TS states satifying an automaton transition
    //fsytrans = vector< vector< vector<int> > >(qtot,vector< vector<int> >(qtot) );
    
    /*for(int j=0;j<qtot;j++)
	   {
	      if(j==current_automaton_state && !neg_trans_to_neighbour[j])
	         continue;
              for(int pro=0;pro<prod_states.size();pro++)
	      { 
		if(cost_filter[n_prod_st].find(prod_states[pro])==cost_filter[n_prod_st].end())
		   continue;
	        for(int z=0;z<negtrans[current_automaton_state][j].size();z++)
	        {
		  for(int lp=0;lp<cost_filter[n_prod_st][prod_states[pro]].size();lp++)
	          {*/
    for(int nstate=0;nstate<prod_states.size();nstate++)
    {
        for(j=0;j<prod_states.size();j++)
        {
            for(z=0;z<cost_filter[prod_states[nstate]][prod_states[j]].size();z++)
            {
                vector<int> transition_req = cost_filter[prod_states[nstate]][prod_states[j]][z];
                if(transition_req.size()==0)
                    continue;

                for(i=0;i<pos_system_state.size();i++)
                {

                    point pt;
                    //transition
                    pt.y =  pos_system_state[i][0];
                    pt.x =  pos_system_state[i][1];

                    v = vector<int>(0);
                    v.push_back(pt.y);
                    v.push_back(pt.x);

                    vector<int> prop_cur;
                    s = conv_vec_to_string(v);
                    if(prop.find(s)==prop.end())
                    {
                        prop_cur.push_back(0);
                    }
                    else
                    {
                        prop_cur = prop[s];
                    }


                    int pos=0;
                    cnt=0;
                    for(int k=0;k<transition_req.size();k++)
                    {
                        if(transition_req[k]>=0)
                            for(int l=0;l<prop_cur.size();l++)
                            {
                                if(transition_req[k]==prop_cur[l])
                                    {cnt++; pos=1; break;}
                            }
                            else
                            {
                                int tmp=1;
                                for(int l=0;l<prop_cur.size();l++)
                                {
                                    if(-1*transition_req[k]==prop_cur[l])
                                        {tmp=0; break;}
                                }
                                cnt+=tmp;
                                if(tmp==0)
                                    break;
                            }
                    }
                        //if(pos && cnt==transition_req.size() ) added
                        //system states from self loop in automata included
                    if(pos && cnt==transition_req.size())
                    {
                        //cout<<prod_states[nstate]<<" "<<prod_states[j]<<" "<<i<<"\n";
                        fsystate[prod_states[j]].push_back(i);
                        //qinstate[j].push_back(nstate);
                        fsytrans[prod_states[nstate]][prod_states[j]].push_back(i);
                        //tmp_cnt++;
                    }

                }
            }
        }
    }
}



int printpath(prod_graph_node* nd)
{
    //cout<<"\n\n";
    if(nd!=NULL)
    {
        //pathlen++;
        int path_len = printpath(nd->par);
        //cout<<"\n......"<<nd->coord.y<<","<<nd->coord.x<<","<<nd->state<<","<<nd->pstate<<"............\n";

        if(nd->par!=NULL)
            return path_len+astar_path_src_to_dest(nd->par,nd);
    }
    return 0;
}



void modifyedges(prod_graph_node* nd,int &total_updates)
{
    
    if(nd!=NULL)
    {
        
        modifyedges(nd->par,total_updates);

        if(nd->par!=NULL)
        {
            int par_node=nd->par->pstate;
            int cur_node=nd->pstate;
           
            //aprog-10
            point par_coord,cur_coord;

            par_coord = nd->par->coord;
            cur_coord = nd->coord;
            
            if(!updated[par_node][cur_node])
            {
                int v,k,it,par_automaton_state = nd->par->state,cur_automaton_state = nd->state,undesired_prop,grid_state;
                unordered_map<long long int,int> obs1;
                int par_cfstate = nd->par->cfstate;
		int cur_cfstate = nd->cfstate;
                if(negtrans[par_automaton_state][par_automaton_state].size() > 0)
                {
                    //adding obstacles on negative self loops
                    vector<int> transition_req = negtrans[par_automaton_state][par_automaton_state][0];
		    //cout<<"\n";
                    for(it=0;it<transition_req.size();it++)
                    {
			//cout<<transition_req[it]<<" ";
                        undesired_prop = transition_req[it]*-1;
                        for(v=0;v<prop_sys_states[undesired_prop].size();v++)
                        {
                            obs1[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]]=-1;
                        }
                    }
		    //cout<<"\n";
                    for(int ind=1;ind<negtrans[par_automaton_state][par_automaton_state].size();ind++)
                    {
                        unordered_map<long long int,int> obs2;
                        vector<int> transition_req = negtrans[par_automaton_state][par_automaton_state][ind];
			//vector<int> transition_req = negtrans[par_automaton_state][par_automaton_state][0];
                        for(it=0;it<transition_req.size();it++)
                        {
                            undesired_prop = transition_req[it]*-1;
                            for(v=0;v<prop_sys_states[undesired_prop].size();v++)
                            {
                                if(obs1[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]])
                                   obs2[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]]=-1;
                            }
                        }
                        obs1 = obs2;
                    }
                    for(int z=0;z<qsytrans[par_automaton_state][par_automaton_state].size();z++)
                    {
                        grid_state = qsytrans[par_automaton_state][par_automaton_state][z];
			//cout<<pos_system_state[grid_state][0]<<" "<<pos_system_state[grid_state][1]<<" ,";
                        obs1[(pos_system_state[grid_state][0])*sz+pos_system_state[grid_state][1]]=0;
                    }
		    if(par_automaton_state != cur_automaton_state)
		    {
			vector<int> transition_req2 = neg_trans_filter[par_cfstate][cur_cfstate][0];
			//transition_req.insert(transition_req.end(),transition_req2.begin(),transition_req2.end());
			//cout<<"\n";
			for(it=0;it<transition_req2.size();it++)
                    	{
				//cout<<transition_req2[it]<<" ";
		                undesired_prop = transition_req2[it]*-1;
		                for(v=0;v<prop_sys_states[undesired_prop].size();v++)
		                {
		                    obs1[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]]=-1;
		                }
			}
			//cout<<"\n";
			/*for(int ind=1;ind<neg_trans_filter[par_cfstate][cur_cfstate].size();ind++)
                    	{
		                unordered_map<long long int,int> obs2;
		                vector<int> transition_req = neg_trans_filter[par_cfstate][cur_cfstate][ind];
				//vector<int> transition_req = negtrans[par_automaton_state][par_automaton_state][0];
		                for(it=0;it<transition_req.size();it++)
		                {
		                    undesired_prop = transition_req[it]*-1;
		                    for(v=0;v<prop_sys_states[undesired_prop].size();v++)
		                    {
		                        if(obs1[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]])
		                           obs2[(prop_sys_states[undesired_prop][v][0])*sz+prop_sys_states[undesired_prop][v][1]]=-1;
		                    }
		                }
		                obs1 = obs2;
                    	}*/
			for(int z=0;z<fsytrans[par_cfstate][cur_cfstate].size();z++)
		        {
		             grid_state = fsytrans[par_cfstate][cur_cfstate][z];
			     //cout<<pos_system_state[grid_state][0]<<" "<<pos_system_state[grid_state][1]<<"\n";
		             obs1[(pos_system_state[grid_state][0])*sz+pos_system_state[grid_state][1]]=0;
		        }
			//cout<<"\n";
		    }

                    copy_map(obs1,-1);    
                    float tmp_dis = cal_actual_distance_bw_points(par_coord,cur_coord);
		    //cout<<"dist:- "<<tmp_dis<<" ("<<par_coord.y<<","<<par_coord.x<<") to ("<< cur_coord.y<<","<<cur_coord.x<<") ";
		    //cout<<par_automaton_state<<" "<<nd->state<<" "<<par_cfstate<<" "<<cur_cfstate<<"\n";
                    if(!updated[par_node][cur_node] || tmp_dis < adj[par_node][cur_node] )
                    {
                        updated[par_node][cur_node] = 1;
                        adj[par_node][cur_node] = tmp_dis;
                    }
                    
                    copy_map(obs1,0);
                }
                else
                {
                    adj[par_node][cur_node] = cal_actual_distance_bw_points(par_coord,cur_coord);
                    updated[par_node][cur_node] = 1;
                }
                total_updates++;
            }
            else if(adj[par_node][cur_node]>=2)
                comp_red++;
            if(adj[par_node][cur_node]>=2)
                tot_comp++;
        }

    }
}


/**assign a id/pstate number to each product graph node**/
int assign_systate_no(long long int coord_value)
{
    systate_no[coord_value] = total_system_states;
    total_system_states++;
    //cout<<"\n";
    
    if(total_system_states>adj.size())
    {
  
        int cur_size = adj.size();
        adj.resize(2*cur_size);
        updated.resize(2*cur_size);
    }
    return total_system_states-1;
}



void expand_prod_graph_node(priority_queue<prod_graph_node*,vector<prod_graph_node*>,prod_graph_comparator> &qopen,prod_graph_node* cur_nd,unordered_map<long long int,prod_graph_node*> &init_vertex,int cfs, int pre)
{
    int zeroloop=0,negloop=0;
    int current_automaton_state = cur_nd->state;
    int n_prod_st = cur_nd->cfstate;
    point cur_pt=cur_nd->coord;

    //check whether exists a self transition on 1 of current_automaton_state//
    if(trans[current_automaton_state][current_automaton_state].size()==1 && trans[current_automaton_state][current_automaton_state][0].size()==1 && trans[current_automaton_state][current_automaton_state][0][0]==0)
        zeroloop=1;
    //check whether exists a self transition onconjunction of neg literals of current_automaton_state//
    if(negtrans[current_automaton_state][current_automaton_state].size()>0) 
        negloop=1;

    point nbh;
    int cur_node = cur_nd->pstate,neigh_state;
    vector<int> cur_node_info{cur_pt.y,cur_pt.x,cur_nd->state,cur_nd->cfstate};
    //cout<<cur_pt.y<<","<<cur_pt.x<<","<<cur_nd->state<<","<<cur_nd->cfstate<<"->"<<cur_nd->f<<"..."<<qtot<<"\n";

    //for every outgoing automaton transition to a neighbour automaton state j//
   
	//cout<<cur_pt.y<<","<<cur_pt.x<<","<<cur_nd->state<<"\n";
   int lps1=0,lps2=0;
   if(node_neighbour.find(key(cur_node_info))==node_neighbour.end())
   { 
		    
		    for(int pro=0;pro<prod_states.size();pro++)
		    {
			if(cost_filter[n_prod_st].find(prod_states[pro])==cost_filter[n_prod_st].end())
			   continue;
			for(int lp=0;lp<cost_filter[n_prod_st][prod_states[pro]].size();lp++)
	          	{
			   vector<int> req = cost_filter[n_prod_st][prod_states[pro]][lp];
			   int xy,cnt=0;
			   for(xy=0;xy<req.size();xy++)
			   {
				if(req[xy]<=0)
				  cnt++;
			   }
			   if(cnt==xy)
			   {
				for(int j=0;j<qtot;j++)
				{
					if(trans[current_automaton_state][j].size()==0)
						continue;
					vector<int> req1 = trans[current_automaton_state][j][0];
		     			vector<int> req2 = cost_filter[n_prod_st][prod_states[pro]][lp];
					vector<int> v,transition_req(req1);
		     			transition_req.insert(transition_req.end(),req2.begin(),req2.end());
					if(has_duplicate(transition_req))
					{ lps2=1;continue;}
					for(int z=0;z<qsytrans[current_automaton_state][j].size();z++)
			    		{
						lps1=1;
					       neigh_state = qsytrans[current_automaton_state][j][z];
					       vector<int> neigh_node_info{pos_system_state[neigh_state][0],pos_system_state[neigh_state][1],j,prod_states[pro]};
					       nbh.y = neigh_node_info[0];
					       nbh.x = neigh_node_info[1];
					       if(!valid(nbh.y,nbh.x) || (!negloop && !zeroloop && cal_heuristic_cost(cur_pt,nbh)>1) )
						   continue;
					       prod_graph_node neigh_nd;
					       neigh_nd.coord = nbh;
					       neigh_nd.state = j;
					       neigh_nd.cfstate = prod_states[pro];
					    //if(node_neighbour.find(key(cur_node_info))==node_neighbour.end())
					       node_neighbour[key(cur_node_info)].push_back(neigh_nd);
					}
				}
			   }
			   else
			   {
				for(int z=0;z<fsytrans[n_prod_st][prod_states[pro]].size();z++)
	            		{
					neigh_state = fsytrans[n_prod_st][prod_states[pro]][z];
					vector<int> neigh_node_info{pos_system_state[neigh_state][0],pos_system_state[neigh_state][1],current_automaton_state,prod_states[pro]};
					nbh.y = neigh_node_info[0];
					nbh.x = neigh_node_info[1];
					if(!valid(nbh.y,nbh.x) || (!negloop && !zeroloop && cal_heuristic_cost(cur_pt,nbh)>1) )
					     continue;
					prod_graph_node neigh_nd;
					neigh_nd.coord = nbh;
					neigh_nd.state = current_automaton_state;
					neigh_nd.cfstate = prod_states[pro];
					//if(node_neighbour.find(key(cur_node_info))==node_neighbour.end())
					node_neighbour[key(cur_node_info)].push_back(neigh_nd);
				}
				//if(negloop && (cost_filter[n_prod_st][n_prod_st].size() == 1))
				  // if(cost_filter[n_prod_st][n_prod_st][0].size()==1 && cost_filter[n_prod_st][n_prod_st][0][0]==0)
				   //{ cout<<"hi\n";	hi=1;}
				
			   }
			}
		    }
      	    
	   for(int j=0;j<qtot;j++)
	   {
		
	      if(j==current_automaton_state && !neg_trans_to_neighbour[j])
	         continue;
//for all outgoing automaton transitions on conjunction of negative literals check whether a neighbour of cur_pt in grid satisfies the trans condn//
              for(int pro=0;pro<prod_states.size();pro++)
	      { 
		if(cost_filter[n_prod_st].find(prod_states[pro])==cost_filter[n_prod_st].end())
		   continue;
	        for(int z=0;z<negtrans[current_automaton_state][j].size();z++)
	        {
		  for(int lp=0;lp<cost_filter[n_prod_st][prod_states[pro]].size();lp++)
	          {
                     vector<int> req1 = negtrans[current_automaton_state][j][z];
		     vector<int> req2 = cost_filter[n_prod_st][prod_states[pro]][lp];
                     vector<int> v,transition_req(req1);
		     transition_req.insert(transition_req.end(),req2.begin(),req2.end());
	             //vector<int> transition_req = negtrans[current_automaton_state][j][z];

	            for(int i=0;i<dir;i++)
	            {
	                if(!valid(cur_pt.y+nb[i][0],cur_pt.x+nb[i][1]))
	                    continue;
                        if( abs(nb[i][0])+abs(nb[i][1])==2)// || (!valid(cur_pt.y+nb[i][0],cur_pt.x) || !valid(cur_pt.y,cur_pt.x+nb[i][1]) ) )
                            continue;

	                nbh.y =  cur_pt.y+nb[i][0];
	                nbh.x =  cur_pt.x+nb[i][1];

	                vector<int> neigh_grid_cell{nbh.y,nbh.x};
	                string nbh_s = conv_vec_to_string(neigh_grid_cell);
	                vector<int> nxtstate;
	                vector<int> prop_nbh;
	                
	                //finding the propositions true at neighbour cell nbh in grid//
	                if(prop.find(nbh_s)==prop.end())
	                {
	                    prop_nbh.push_back(0);
	                }
	                else
	                {
	                    prop_nbh = prop[nbh_s];
	                    prop_nbh.push_back(0);
	                }

	                int trans_literals_satisfied=0;
	                for(int k=0;k<transition_req.size();k++)
	                {
	                    if(transition_req[k]>=0)
	                        for(int l=0;l<prop_nbh.size();l++)
	                        {
	                            if(transition_req[k]==prop_nbh[l])
	                            {
	                            trans_literals_satisfied++; break;
	                            }
	                        }
	                    else
	                    {
	                        int satisfied_neg_literal=1;
	                        for(int l=0;l<prop_nbh.size();l++)
	                        {
	                            if(-1*transition_req[k]==prop_nbh[l])
	                            {
	                                satisfied_neg_literal=0; break;
	                            }
	                        }
	                        trans_literals_satisfied+=satisfied_neg_literal;
	                    }
	                }
	                if(trans_literals_satisfied==transition_req.size())
	                {
	                            prod_graph_node neigh_nd;
			            neigh_nd.coord = nbh;
			            neigh_nd.state = j;
                                    neigh_nd.cfstate = prod_states[pro];
				    //neigh_nd.t_cst = trans_cost[prod_states[pro]];
			            node_neighbour[key(cur_node_info)].push_back(neigh_nd);
	                }
	            }
		  }
	        }
	      }
	    }
	}
	

    vector<prod_graph_node> neigh_set;
    neigh_set = node_neighbour[key(cur_node_info)];
    //cout<<"------neighbour set  start--------\n";
    for(int i=0;i<neigh_set.size();i++)
    {
        vector<int> neigh_nd{neigh_set[i].coord.y,neigh_set[i].coord.x,neigh_set[i].state,neigh_set[i].cfstate};
	float cost_val;   
	unsigned long long int coord_value = key(neigh_nd); 
        if(systate_no.find(coord_value)==systate_no.end())
            assign_systate_no(coord_value);
        point nbh = neigh_set[i].coord;
        long long int neigh_state = systate_no[coord_value];

	
	cost_val = cal_heuristic_cost(cur_pt,nbh);

	//cout<<cost_val<<"\n";

        if(!updated[cur_node][neigh_state])
            adj[cur_node][neigh_state] = cost_val;
        if(adj[cur_node][neigh_state]==-1)
            continue;
        
        float tval = adj[cur_node][neigh_state];
	//cout<<tval<<"\n";
	//cout<<neigh_set[i].coord.y<<" "<<neigh_set[i].coord.x<<" "<<neigh_set[i].state<<" "<<neigh_set[i].cfstate<<" "<<"-->";
        if(init_vertex.find(key(neigh_nd))==init_vertex.end())
        {

            prod_graph_node* tmp = new_prod_graph_node(neigh_state,neigh_set[i].state,nbh.y,nbh.x,neigh_set[i].cfstate);
            tmp->g = cur_nd->g+tval;
            tmp->h = 0;
            tmp->f = tmp->g+tmp->h;
	    //cout<<tmp->f<<"\n";
            tmp->par = cur_nd;
	    tmp->t_cst = neigh_set[i].t_cst;
            init_vertex[key(neigh_nd)]=tmp;
            qopen.push(tmp);

        }
        else
        {
            prod_graph_node* oldtmp = init_vertex[key(neigh_nd)];
            
            if(oldtmp->g > (cur_nd->g+tval) )
            {
                prod_graph_node* tmp = new_prod_graph_node(neigh_state,neigh_set[i].state,nbh.y,nbh.x,neigh_set[i].cfstate);
                tmp->g = cur_nd->g+tval;
                tmp->h = 0;
                tmp->f = tmp->g+tmp->h;
		//cout<<tmp->f<<"\n";
		tmp->t_cst = neigh_set[i].t_cst;
                tmp->par = cur_nd;
                init_vertex[key(neigh_nd)]=tmp;
                qopen.push(tmp);
            }

        }

    }
    //cout<<"------neighbour set  ends--------\n";
}




int find_path_in_prod_graph(prod_graph_node* srcnode,prod_graph_node* dest_node,int degen)
{
    int iter;
    //iterate till we get a path where no edge cost has been modified//
    for(iter=1; iter>0 ;iter++)
    {
        unordered_map<long long int ,prod_graph_node*> init_vertex;
        unordered_map<long long int ,int> vis;
        int destreach=0;
        prod_graph_node* st = new_prod_graph_node(srcnode->pstate,srcnode->state,srcnode->coord.y,srcnode->coord.x,srcnode->cfstate);
        priority_queue<prod_graph_node*,vector<prod_graph_node*>,prod_graph_comparator> qopen;
        point cur_pt = st->coord;
        vector<int> cur_node_info{cur_pt.y,cur_pt.x,st->state,st->cfstate};
        long long int coord_value = key(cur_node_info); 
        
        if(systate_no.find(coord_value)==systate_no.end())
           assign_systate_no(coord_value);
        
        st->pstate = systate_no[coord_value];
        int path_valid=0;
        vector<prod_graph_node*> closed;
        qopen.push(st);
        //if zero length paths are allowed//
        if(degen)
        {
        init_vertex[key(cur_node_info)]=st;
        path_valid=1;
        }

        //path_valid denotes path length is greater than 0 or path length of 0 is allowed in case of finding prefix only//
        while(!qopen.empty())
        {
            prod_graph_node* cur_nd = qopen.top();
            qopen.pop();
            point cur_pt = cur_nd->coord;
            
            int current_automaton_state = cur_nd->state;
            int current_cost_filter_state = cur_nd->cfstate;
            vector<int> cur_node_info{cur_pt.y,cur_pt.x,cur_nd->state,cur_nd->cfstate};
            
            long long int  s;
            s = key(cur_node_info);

            if(path_valid)
            {
            	if(vis.find(s)!=vis.end())
                	continue;
            	else
                	vis[s]=1;
            }

	    //cout<<"\n"<<cur_pt.y<<" "<<cur_pt.x<<" "<<cur_nd->state<<" "<<cur_nd->cfstate<<" "<<s<<" "<<cur_nd->f<<"\n";

            if(path_valid && cur_nd->coord.y==dest_node->coord.y && cur_nd->coord.x==dest_node->coord.x && cur_nd->state==dest_node->state && cur_nd->cfstate==dest_node->cfstate)
            {
                int total_updates=0;
                path = vector<int>(0);
		//cout<<"\n-----"<<cur_nd->f<<"--------\n";

                modifyedges(cur_nd,total_updates);
                //cout<<" after modifing edges \n";
                destreach=1;
                if(iter>1 && total_updates==0)
                {
                    
                    cur_path = vector<point>(0);
                    iter=-5; 
                    int len = printpath(cur_nd);
                    //cout<<"\nlength="<<len<<"--------\n";
                    if(!degen)
                    {
                        stored_suffix_paths.push_back(cur_path);
                        cur_nd->pstate = stored_suffix_paths.size()-1;
			cout<<cur_nd->f<<"\n";
			cur_nd->f += trans_cost[cur_nd->cfstate];
                        loopf.push(cur_nd);
			prod_graph_node* nd = cur_nd;
			cout<<"\nsufix\n";
			while(nd!=NULL)
			{
				cout<<"-->"<<nd->coord.y<<","<<nd->coord.x<<","<<nd->state<<","<<nd->cfstate<<","<<nd->f<<"\n";
				nd=nd->par;
			}
                    }
                    else
		    { 
                        prefix_length = cur_nd->f;
			prod_graph_node* nd = cur_nd;
			cout<<"\nprefix\n";
			while(nd!=NULL)
			{
				cout<<"-->"<<nd->coord.y<<","<<nd->coord.x<<","<<nd->state<<","<<nd->cfstate<<","<<nd->f<<"\n";
				nd=nd->par;
			}
		    }                
		}
                break;
            }

            if(!path_valid)
                path_valid=1;
	   //cout<<"------------------------destination state"<<dest_node->cfstate<<"\n";
            expand_prod_graph_node(qopen,cur_nd,init_vertex,dest_node->cfstate,degen);
            closed.push_back(cur_nd);
        
        }
        for(int i=0;i<closed.size();i++)
            delete(closed[i]);
        if(!destreach)
          break;
    }   
    return iter;
}

void prefs_violated(int pv)
{
	vector<int> list;
	//cout<<pv<<"\n";
	while(pv!=0)
	{
	   list.push_back(pv%10-1);
	   pv=pv/10;
	}
	int j=1;
	for(int i=list.size()-1;i>=0;i--)
	{
	   //cout<<list[i]<<" dest state :-"<<prefs_dest[i][0]<<" i= "<<i<<"\n";
	   if(find(prefs_dest[i].begin(),prefs_dest[i].end(),list[i]) == prefs_dest[i].end())
		cout<<"preference "<<j<<" is violated\n";
	   j++;
	} 
}
int main(int args,char **argv)
{
    
    t = clock();
    //printf("hi\n");
    initializegrid(argv[1]);

    int cntstate=0;
    calsystates();
    calfilstates();
    pair<int,int> dest_node;
   //cout<<"***************before suffix cycles \n";
    compute_suffix_cycles();
    int k = clock() - t;
   //cout<<"***************after suffix cycles \n";
    //loopf all suffix cycle starting nodes
    int pref_cst,pre_path_cst,suf_path_cst,pv;
    while(!loopf.empty())
    {
      prod_graph_node* finstate = loopf.top();
      //cout<<"........"<<finstate->cfstate<<"   ,  "<<trans_cost[finstate->cfstate]<<"\n";
      //cout<<"\n............................................................................................................\n";
      prod_graph_node* st = new_prod_graph_node(0,0,0,0,prod_states[0]);
      loopf.pop();
      int iter,cnt=0;
      iter = find_path_in_prod_graph(st,finstate,1);
      
      //suffix_path = stored_suffix_paths[finstate->pstate];
      
      if(iter < 0)
      {
	//prod_graph_node* nd = finstate;
        prefix_path = cur_path;
        suffix_path = stored_suffix_paths[finstate->pstate];
        suf_len = finstate->f;
	//pv = finstate->cfstate;
	//pref_cst=trans_cost[finstate->cfstate];
        break;
      }
      //node_neighbour.clear();
    }
    
    //cout<<"\npreference cost "<<pref_cst<<"\n\n";
    //cout<<"------preferences violated------\n";
    //prefs_violated(pv);
    cout<<"\nlength_prefix = "<<prefix_length<<"\n";
    //cout<<"prefix path length = "<<prefix_length-pref_cst<<"\n";
    cout<<"\nlength_suffix="<<suf_len<<"\n";
    //cout<<"suffix path length = "<<suf_len-pref_cst<<"\n";
    cout<<"\nprefix\n";
    for(int i=0;i<prefix_path.size();i++)
        cout<<"**"<<prefix_path[i].y<<","<<prefix_path[i].x<<"\n";//<<prefix_path[i].s<<","<<prefix_path[i].cf<<","<<prefix_path[i].f<<"\n";

    cout<<"\nsuffix\n";
    for(int i=0;i<suffix_path.size();i++)
        cout<<"**"<<suffix_path[i].y<<","<<suffix_path[i].x<<"\n";//<<suffix_path[i].s<<","<<suffix_path[i].cf<<","<<suffix_path[i].f<<"\n";

    cout<<"\ntotal_product_graph_nodes="<<total_system_states;
    cout<<"\ncomp_red="<<comp_red;
    cout<<"\ntot_comp="<<tot_comp;
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
    double time_taken1 = ((double)k)/CLOCKS_PER_SEC; // in seconds
    printf("\nprogram took %f seconds to execute \n", time_taken1);
    printf("\nprogram took %f seconds to execute \n", time_taken);
    cout<<"\n..............................................\n\n";
}

pair<int,int> compute_suffix_cycles()
{
    int iter,i,suflen=INT_MAX;
    pair<int,int> dest_node;

    /*******for every final state of automaton***************/
    for(int fs=0;fs<dest.size();fs++)
    {
	//cout<<"hi\n";
       vector<int> vis_sy_state(pos_system_state.size());
       /*******for every sytem state associated with the given automaton final state dest[fs]*********/
       for(i=0;i<qsystate[dest[fs]].size();i++)
       {
	 for(int pro=0;pro<prod_states.size();pro++)
         {
            int systate = qsystate[dest[fs]][i];
	    // cout<<"\n"<<systate<<" "<<pos_system_state[systate][0]<<" "<<pos_system_state[systate][1]<<" "<<dest[fs]<<" "<<prod_states[pro]<<"\n";
            /*if(vis_sy_state[systate]==1)
                continue;
            else
                vis_sy_state[systate]=1;*/
            prod_graph_node* st = new_prod_graph_node(systate,dest[fs],pos_system_state[systate][0],pos_system_state[systate][1],prod_states[pro]);
            //cout<<"............. before find path starts................ \n\n";
            find_path_in_prod_graph(st,st,0);
	    //cout<<"............. after find path ends ................\n\n";
        }
      }
    }
    //\\************************************************************************************//

    return dest_node;
}
