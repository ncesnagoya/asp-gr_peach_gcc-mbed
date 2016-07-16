/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2007-2011 by TAKAGI Nobuhisa
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 */
#include <fstream>
#include <sstream>
#include "toppers/diagnostics.hpp"
#include "toppers/s_record.hpp"
#include "toppers/macro_processor.hpp"
#include "toppers/itronx/component.hpp"
#include "cfg.hpp"
#include <boost/spirit/include/classic.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

/*!
 *  \brief  オブジェクトID番号の割付け
 *  \param[in]  api_map   ソースに記述された静的APIを登録したコンテナ
 */
void assign_id( toppers::itronx::cfg1_out::static_api_map& api_map )
{
  using namespace toppers;
  using namespace toppers::itronx;

  std::string id_input_file( get_global< std::string >( "id-input-file" ) );
  if ( id_input_file.empty() )  // --id-input-file オプションが指定されていない場合...
  {
    for ( cfg1_out::static_api_map::iterator iter( api_map.begin() ), last( api_map.end() );
          iter != last;
          ++iter )
    {
      static_api::assign_id( iter->second.begin(), iter->second.end() );
    }
  }
  else  // --id-input-file オプションが指定されている場合...
  {
    typedef std::map< std::string, std::pair< long, bool > > id_map_t;
    id_map_t id_map;
    std::ifstream ifs( id_input_file.c_str() );
    while ( ifs )
    {
      std::string linebuf;
      std::getline( ifs, linebuf );
      if ( ifs.bad() )
      {
        fatal( _( "I/O error" ) );
      }
      if ( linebuf.empty() || linebuf == "\r" )
      {
        break;
      }

      std::istringstream iss( linebuf );
      std::string name;
      iss >> name;
      if ( iss.fail() )
      {
        fatal( _( "id file `%1%\' is invalid" ), id_input_file );
      }

      long value;
      iss >> value;
      if ( iss.fail() )
      {
        fatal( _( "id file `%1%\' is invalid" ), id_input_file );
      }

      if ( id_map.find( name ) != id_map.end() )
      {
        fatal( _( "E_OBJ: `%1%\' is duplicated" ), name );
      }
      else
      {
        id_map[ name ] = std::make_pair( value, false );
      }
    }

    for ( cfg1_out::static_api_map::iterator iter( api_map.begin() ), last( api_map.end() );
          iter != last;
          ++iter )
    {
      for ( std::vector< static_api >::iterator iter2( iter->second.begin() ), last2( iter->second.end() );
            iter2 != last2;
            ++iter2 )
      {
        static_api::info const* info = iter2->get_info();
        if ( info->id_pos >= 0 )
        {
          std::string name( iter2->at( info->id_pos ).text );
          std::string symbol( iter2->at( info->id_pos ).symbol );
          if ( !info->slave && symbol[0] == '#' )
          {
            id_map_t::iterator hit( id_map.find( name ) );
            if ( hit != id_map.end() )
            {
              long id_value = hit->second.first;
              if ( id_value > 0 )
              {
                iter2->at( info->id_pos ).value = id_value;
                hit->second.second = true;
              }
            }
          }
        }
      }
      static_api::assign_id( iter->second.begin(), iter->second.end() );
    }

    for ( id_map_t::const_iterator iter( id_map.begin() ), last( id_map.end() ); iter != last; ++iter )  // 残り物があれば...
    {
      if ( !iter->second.second )
      {
        warning( _( "object identifier `%1%\' is not used" ), iter->first );
      }
    }
  }

  // --id-output-file オプションが指定されている場合
  std::string id_output_file( get_global< std::string >( "id-output-file" ) );
  if ( !id_output_file.empty() )
  {
    std::ofstream ofs( id_output_file.c_str() );
    for ( cfg1_out::static_api_map::iterator iter( api_map.begin() ), last( api_map.end() );
      iter != last;
      ++iter )
    {
      for ( std::vector< static_api >::const_iterator iter2( iter->second.begin() ), last2( iter->second.end() );
            iter2 != last2;
            ++iter2 )
      {
        static_api::info const* info = iter2->get_info();
        if ( info->id_pos >= 0 )
        {
          std::string name( iter2->at( info->id_pos ).text );
          std::string symbol( iter2->at( info->id_pos ).symbol );
          if ( !info->slave && symbol[0] == '#' )
          {
            ofs << name << '\t' << iter2->at( info->id_pos ).value.get() << std::endl;
          }
        }
      }
    }
  }
}


/*!
 *  \brief  マクロプロセッサに登録するID割付け関数
 *  \param[in]  line      行番号
 *  \param[in]  arg_list  マクロ実引数リスト
 *  \param[in]  p_ctx     マクロコンテキスト
 *  \retval     マクロ返却値
 */
toppers::macro_processor::var_t bf_assignid( toppers::text_line const& line,
                                             std::vector< toppers::macro_processor::var_t > const& arg_list,
                                             toppers::macro_processor::context* p_ctx )
{
  using namespace toppers;
  using namespace toppers::itronx;
  using toppers::text_line;
  typedef toppers::macro_processor::element element;
  typedef toppers::macro_processor::var_t var_t;
  typedef toppers::macro_processor::context context;

  return var_t();
}

//! 組み込み関数ASSIGNIDを登録する
void register_bf_assignid( toppers::macro_processor* mproc )
{
  // ↓ 追加組み込み関数の登録
  toppers::macro_processor::func_t func_info = {};
  func_info.name = "ASSIGNID";
  func_info.f = &bf_assignid;
  mproc->add_builtin_function( func_info );
}

/*!
 *  \brief  パス２処理
 *  \retval true  成功
 *  \retval false 失敗
 */
bool cfg2_main()
{
  using namespace toppers;
  using namespace toppers::itronx;

  std::string kernel( get_global< std::string >( "kernel" ) );
  itronx::factory factory( kernel );
  global( "factory" ) = &factory;

  // *.cfgとcfg1_out.srecの読み込み
  std::string input_file;
  try
  {
    input_file = get_global< std::string >( "input-file" );
  }
  catch ( boost::bad_any_cast& )
  {
    fatal( _( "no input files" ) );
  }
  std::string cfg1_out_name( get_global< std::string >( "cfg1_out" ) );
  std::auto_ptr< cfg1_out > cfg1_out( factory.create_cfg1_out( cfg1_out_name ) );

  codeset_t codeset = get_global< codeset_t >( "codeset" );
  cfg1_out->load_cfg( input_file, codeset, *factory.get_static_api_info_map() );
  cfg1_out->load_srec();

  std::auto_ptr< macro_processor > mproc;
  std::auto_ptr< component > component_ptr;

  if ( get_global< bool >( "with-software-components" ) )
  {
    mproc = factory.create_macro_processor( *cfg1_out, cfg1_out->get_static_api_array() );
    component_ptr.reset( new component( mproc.get() ) );
  }
  else  // 従来仕様（ソフトウェア部品非対応）
  {
    cfg1_out::static_api_map api_map( cfg1_out->merge() );
    assign_id( api_map ); // ID番号の割付け
    mproc = factory.create_macro_processor( *cfg1_out, api_map );
  }

  // テンプレート処理
  boost::any template_file( global( "template-file" ) );
  namespace fs = boost::filesystem;
//  fs::path cfg_dir( get_global< std::string >( "cfg-directory" ), fs::native );
  fs::path cfg_dir( get_global< std::string >( "cfg-directory" ) );  // filesystem3対応
  std::vector< std::string > include_paths = get_global< std::vector< std::string > >( "include-path" );
//  include_paths.push_back( cfg_dir.empty() ? "." : cfg_dir.native_file_string() );
  include_paths.push_back( cfg_dir.empty() ? "." : cfg_dir.string() );  // filesystem3対応
  if ( !template_file.empty() )
  {
    toppers::text in_text;
    toppers::text pp_text;
    std::string file_name( boost::any_cast< std::string& >( template_file ) );

    in_text.set_line( file_name, 1 );
    std::ifstream ifs( file_name.c_str() );
    in_text.append( ifs );
    macro_processor::preprocess( in_text, pp_text );
    mproc->evaluate( pp_text );
  }
  else  // テンプレートファイルが指定されていないので、共通部分（kernel.tf）のみを処理
  {
    fs::path kernel_cfg_template_file( cfg_dir/fs::path( "../../kernel/kernel.tf" ) );
    if ( !fs::exists( kernel_cfg_template_file ) )
    {
//      error( _( "cannot open file `%1%\'" ), kernel_cfg_template_file.native_file_string() );
      error( _( "cannot open file `%1%\'" ), kernel_cfg_template_file.string() );  // filesystem3対応
    }
    else
    {
      toppers::text in_text;
      toppers::text pp_text;

//      in_text.set_line( kernel_cfg_template_file.native_file_string(), 1 );
      in_text.set_line( kernel_cfg_template_file.string(), 1 );  // filesystem3対応
//      std::ifstream ifs( kernel_cfg_template_file.native_file_string().c_str() );
      std::ifstream ifs( kernel_cfg_template_file.string().c_str() );  // filesystem3対応
      in_text.append( ifs );
      macro_processor::preprocess( in_text, pp_text );
      mproc->evaluate( pp_text );
    }
  }

  if ( get_error_count() > 0 )
  {
    return false;
  }
  output_file::save();
  return true;
}
