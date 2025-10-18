
# - TARGET: 타겟 이름
# - ...: 셰이더 파일들
function(compile_shader TARGET)
    set(SHADER_VERSION 6_0)    
    foreach(SHADER IN LISTS ${ARGV})
        # 확장자로 셰이더 타입 구분
        if (SHADER MATCHES ".vs$")
            set(SHADER_PROFILE vs_${SHADER_VERSION})
            set(SHADER_VARIABLE_POSTFIX VertexShader)
        elseif (SHADER MATCHES ".ps$")
            set(SHADER_PROFILE ps_${SHADER_VERSION})
            set(SHADER_VARIABLE_POSTFIX PixelShader)
        else ()
            message(FATAL_ERROR "Invalid shader file: ${SHADER}")
        endif()

        # 옵션 설정
        get_filename_component(SHADER_HEADER_FILE_NAME ${SHADER} NAME)
        get_filename_component(SHADER_HEADER_FILE_NAME_WE ${SHADER} NAME_WE)
        set(SHADER_COMPILE_OPTIONS
            /nologo
            /Emain
            /T${SHADER_PROFILE}
            /Fh ${CMAKE_CURRENT_SOURCE_DIR}/Src/Shader/${SHADER_HEADER_FILE_NAME}.h
            /Vn ${SHADER_HEADER_FILE_NAME_WE}${SHADER_VARIABLE_POSTFIX}
            ${SHADER}
        )

        # 컴파일
        add_custom_command(
            TARGET ${TARGET} PRE_BUILD
            COMMAND ${CMAKE_SOURCE_DIR}/External/DXC/x64/dxc.exe ${SHADER_COMPILE_OPTIONS}
            COMMENT "Compiling shader: ${SHADER_HEADER_FILE_NAME}"
        )
    endforeach()
endfunction()
