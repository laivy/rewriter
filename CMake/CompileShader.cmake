
# - TARGET: 타겟 이름
# - ...: 셰이더 파일들
function(target_compile_shader target_name)
    include(FetchContent)
    FetchContent_Populate(
        DirectXShaderCompiler
        URL https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2505.1/dxc_2025_07_14.zip
        URL_HASH SHA256=9ad895a6b039e3a8f8c22a1009f866800b840a74b50db9218d13319e215ea8a4
        SOURCE_DIR ${RWT_EXTERNAL_DIR}/DirectXShaderCompiler
        DOWNLOAD_NO_PROGRESS TRUE
    )

    set(shader_version 6_0)
    foreach(shader IN LISTS ${ARGV})
        # 확장자로 셰이더 타입 구분
        if(shader MATCHES ".vs$")
            set(shader_profile vs_${shader_version})
            set(shader_variable_postfix VertexShader)
        elseif(shader MATCHES ".ps$")
            set(shader_profile ps_${shader_version})
            set(shader_variable_postfix PixelShader)
        else()
            message(FATAL_ERROR "Invalid shader file: ${shader}")
        endif()

        # 옵션 설정
        get_filename_component(shader_header_file_name ${shader} NAME)
        get_filename_component(shader_header_file_name_we ${shader} NAME_WE)
        set(compile_options
            /nologo
            /Emain
            /T${shader_profile}
            /Fh ${CMAKE_CURRENT_SOURCE_DIR}/Src/Shader/${shader_header_file_name}.h
            /Vn ${shader_header_file_name_we}${shader_variable_postfix}
            ${shader}
        )

        # 컴파일
        add_custom_command(
            TARGET ${target_name} PRE_BUILD
            COMMAND ${directxshadercompiler_SOURCE_DIR}/bin/x64/dxc.exe ${compile_options}
            COMMENT "Compiling shader: ${shader_header_file_name}"
        )
    endforeach()
endfunction()
